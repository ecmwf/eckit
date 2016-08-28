/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <sys/resource.h>

#include "eckit/config/Resource.h"
#include "eckit/io/Select.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/PipeApplication.h"
#include "eckit/serialisation/PipeStream.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/log/Seconds.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

PipeApplication::PipeApplication(int argc, char** argv,const char* homeenv,
                                 LoggingPolicy* logPolicy,
                                 MonitoringPolicy* monPolicy,
                                 LocationPolicy* locPolicy,
                                 SignallingPolicy* sigPolicy)
    : Application(argc, argv, homeenv, logPolicy, monPolicy, locPolicy, sigPolicy),
      in_(this,"-in",-1),
      out_(this,"-out",-1) {
    // Establish relationship with 'parent' thread for the monitoring

    long parent = Resource<long>("-parent",0);
    Monitor::instance().parent(parent);

    // If we fork, avoid childs having the pipe open
    SYSCALL(fcntl(in_,F_SETFD,FD_CLOEXEC));
    SYSCALL(fcntl(out_,F_SETFD,FD_CLOEXEC));
}

PipeApplication::~PipeApplication() {
}

void PipeApplication::run()
{
    long timeout = Resource<long>("selectTimeout",10*60); // 10 Minutes

    bool debug = false;

    if(getenv("PIPE_DEBUG")) {
        debug = true;
        std::cout << "PIPE_DEBUG[" << name_ << "]" << std::endl;
        if( Application::name() == getenv("PIPE_DEBUG") ) {
            std::cout << "debug me " << getpid() << std::endl;
            ::sleep(10);

            ::sleep(10);
        }
    }

    long maxRequests = Resource<long>("maxRequests",0);
    long maxCpu      = Resource<long>("maxCpu",0);
    long maxUptime   = Resource<long>("maxUptime",0);
    long maxMemory   = Resource<long>("maxMemory",0); // Given in MB
    long maxSwaps    = Resource<long>("maxSwaps",0);

    PipeStream pipe(in_,out_);

    init(pipe);

    int count = 0;

    for(;;) {
        count++;

        waiting();

        Select select(in_);

        if(!select.ready(timeout)) {
            if(!debug) {
                Log::warning() << "Timeout, exiting" << std::endl;
                return;
            }
        }

        try {
            bool end_;
            pipe >> end_; // End of batch marker
            if(end_)
                endBatch();
            else
                process(pipe);
        } catch(std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " <<
                            Here() << std::endl;
            Log::error() << "** Exception is re-thrown" << std::endl;
            pipe << e;
            throw;
        }

        if(debug)
            continue;

        struct rusage usage;
        getrusage(RUSAGE_SELF,&usage);

        time_t uptime = Application::uptime();

        Log::info() << "End of request " << BigNum(count)
                    << ", PID: "         << ::getpid()
                    << ", Uptime: "      << Seconds(uptime)
                    << ", CPU: "         << Seconds(usage.ru_utime.tv_sec + usage.ru_utime.tv_sec)
                    << ", Memory: "      << Bytes(usage.ru_maxrss*1024)
                    << ", Swaps: "       << BigNum(usage.ru_nswap)
                    << std::endl;

        if(maxRequests && (count >= maxRequests)) {
            Log::info() << "Maximum number of requests reached ("
                        << BigNum(maxRequests)
                        << "), exiting" << std::endl;
            return;
        }

        if(maxUptime && (uptime >= maxUptime)) {
            Log::info() << "Maximum uptime reached ("
                        << Seconds(maxUptime)
                        << "), exiting" << std::endl;
            return;
        }

        if(maxCpu && (usage.ru_utime.tv_sec + usage.ru_utime.tv_sec >= maxCpu)) {
            Log::info() << "Maximum CPU usage reached ("
                        << Seconds(maxCpu)
                        << "), exiting" << std::endl;
            return;
        }

        if(maxMemory && (usage.ru_maxrss >= (maxMemory*1024))) {
            Log::info() << "Maximum memory usage reached ("
                        << Bytes(usage.ru_maxrss*1024)
                        << " > "
                        << Bytes(maxMemory * 1024 *1024)
                        << "), exiting" << std::endl;
            return;
        }

        if(maxSwaps && (usage.ru_nswap >= maxSwaps)) {
            Log::info() << "Maximum memory usage reached ("
                        << BigNum(maxSwaps)
                        << "), exiting" << std::endl;
            return;
        }

// #define X(a) Log::info() << "Usage " << #a << ": " << usage.a << std::endl;
#define X(a)
        X(ru_utime.tv_sec);
        X(ru_utime.tv_sec);
        X(ru_maxrss);
        X(ru_ixrss);
        X(ru_idrss);
        X(ru_isrss);
        X(ru_minflt);
        X(ru_majflt);
        X(ru_nswap);
        X(ru_inblock);
        X(ru_oublock);
        X(ru_msgsnd);
        X(ru_msgrcv);
        X(ru_nsignals);
        X(ru_nvcsw);
        X(ru_nivcsw);
    }
}


void PipeApplication::endBatch() {
}


void PipeApplication::init(Stream&) {
}

void PipeApplication::launch(const std::string& name, int input, int output) {
    char in [20]; snprintf(in,20,"%d", input);
    char out[20]; snprintf(out,20,"%d",output);
    char par[20]; snprintf(par,20,"%ld",Monitor::instance().self());

    PathName cmd = std::string("~/bin/") + name;

    Log::debug() << "execlp(" << cmd.localPath() << ','
                 << cmd.baseName().localPath() << ','
                 << "-in," << in << ','
                 << "-out," << out << ','
                 << "-parent," << par << ")" << std::endl;

#if 0
    if(getenv("PIPE_DEBUG")) {
        ::execlp(getenv("PIPE_DEBUG"),getenv("PIPE_DEBUG"),
                 cmd.c_str(), "-in",in, "-out",out, "-parent",par, 0);

        std::cerr << "Exec failed " << getenv("PIPE_DEBUG") << Log::syserr  << std::endl;
        ::kill(getpid(),SIGTERM);
    }
    else
#endif

    char command[1024];
    char basename[1024];

    ASSERT(sizeof(command)-1 > std::string(cmd).length());

    snprintf(command,1024,"%s", cmd.localPath());
    snprintf(basename,1024,"%s", cmd.baseName().localPath());

    ::execlp(command, basename,
             "-in",in,
             "-out",out,
             "-parent",par,
             (void*)0);

    std::cerr << "Exec failed " << cmd << Log::syserr << std::endl;

    // exit is not enough: some destructor of ostore objects
    // are waiting for some locks
    _exit(1);
}

void PipeApplication::waiting() {
    Log::status() << "-" << std::endl;
}

//-----------------------------------------------------------------------------

} // namespace eckit
