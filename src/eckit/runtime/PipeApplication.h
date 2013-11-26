/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file PipeApplication.h
/// @author Baudouin Raoult

#ifndef eckit_PipeApplication_h
#define eckit_PipeApplication_h

#include <sys/resource.h>

#include "eckit/config/Resource.h"
#include "eckit/io/Select.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/PipeApplication.h"
#include "eckit/serialisation/PipeStream.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/types/Seconds.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template< class APP >
class PipeApplication : public APP {

public: // methods

	PipeApplication(int,char**);

	~PipeApplication();

	virtual void process(Stream&) = 0;
	virtual void endBatch();
    virtual void init(Stream&);
	virtual void waiting();	
	
	static void launch(const string& name, int in, int out);

private: // members

	Resource<long> in_;
	Resource<long> out_;

    /// overridden from Application
	virtual void run();

};

//-----------------------------------------------------------------------------

template< class APP >
PipeApplication<APP>::PipeApplication(int argc,char **argv): 
	APP(argc,argv),
	in_(this,"-in",-1),
	out_(this,"-out",-1)
{
	// Establish relationship with 'parent' thread for the monitoring

	long parent = Resource<long>("-parent",0);
	Monitor::instance().parent(parent);

	// If we fork, avoid childs having the pipe open
	SYSCALL(fcntl(in_,F_SETFD,FD_CLOEXEC));
	SYSCALL(fcntl(out_,F_SETFD,FD_CLOEXEC));
}

template< class APP >
PipeApplication<APP>::~PipeApplication()
{
}

template< class APP >
void PipeApplication<APP>::run()
{
	static const char* here = __FUNCTION__;
	long timeout = Resource<long>("selectTimeout",10*60); // 10 Minutes

	bool debug = false;

	if(getenv("PIPE_DEBUG"))
	{
		debug = true;
		std::cout << "PIPE_DEBUG[" << Context::instance().runName() << "]" << std::endl;
        if( APP::name() == getenv("PIPE_DEBUG") )
		{
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

	for(;;)
	{
		count++;

		waiting();

		Select select(in_);

		if(!select.ready(timeout))
		{
			if(!debug)
			{
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
		}
		catch(std::exception& e) {
			Log::error() << "** " << e.what() << " Caught in " <<
				here << std::endl;
			Log::error() << "** Exception is re-thrown" << std::endl;
			pipe << e;
			throw;
		}

		if(debug) 
			continue;


		struct rusage usage;
		getrusage(RUSAGE_SELF,&usage);

		time_t uptime = APP::uptime();

		Log::info() << "End of request " << BigNum(count) 
			        << ", PID: "         << ::getpid()
			        << ", Uptime: "      << Seconds(uptime)
			        << ", CPU: "         << Seconds(usage.ru_utime.tv_sec + usage.ru_utime.tv_sec)
					<< ", Memory: "      << Bytes(usage.ru_maxrss*1024)
					<< ", Swaps: "       << BigNum(usage.ru_nswap)
					<< std::endl;

		if(maxRequests && (count >= maxRequests))
		{
			Log::info() << "Maximum number of requests reached (" 
						<< BigNum(maxRequests)
						<< "), exiting" << std::endl;
			return;
		}

		if(maxUptime && (uptime >= maxUptime))
		{
			Log::info() << "Maximum uptime reached (" 
						<< Seconds(maxUptime)
				        << "), exiting" << std::endl;
			return;
		}

		if(maxCpu && (usage.ru_utime.tv_sec + usage.ru_utime.tv_sec >= maxCpu))
		{
			Log::info() << "Maximum CPU usage reached (" 
						<< Seconds(maxCpu)
				        << "), exiting" << std::endl;
			return;
		}

		if(maxMemory && (usage.ru_maxrss >= (maxMemory*1024)))
		{
			Log::info() << "Maximum memory usage reached (" 
						<< Bytes(usage.ru_maxrss*1024) 
						<< " > "
						<< Bytes(maxMemory * 1024 *1024)
				        << "), exiting" << std::endl;
			return;
		}

		if(maxSwaps && (usage.ru_nswap >= maxSwaps))
		{
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


template< class APP >
void PipeApplication<APP>::endBatch()
{
}


template< class APP >
void PipeApplication<APP>::init(Stream&)
{
}

template< class APP >
void PipeApplication<APP>::launch(const string& name, int input, int output)
{
    
    char in [20]; snprintf(in,20,"%d", input);
    char out[20]; snprintf(out,20,"%d",output);
    char par[20]; snprintf(par,20,"%ld",Monitor::instance().self());

    PathName cmd = string("~/bin/") + name;

    Log::debug() << "execlp(" << cmd.localPath() << ','
                 << cmd.baseName().localPath() << ','
                 << "-in," << in << ','
                 << "-out," << out << ','
                 << "-parent," << par << ")" << std::endl;

#if 0
    if(getenv("PIPE_DEBUG"))
    {
        ::execlp(getenv("PIPE_DEBUG"),getenv("PIPE_DEBUG"),
            cmd.c_str(), "-in",in, "-out",out, "-parent",par, 0);

        std::cerr << "Exec failed " << getenv("PIPE_DEBUG") << Log::syserr  << std::endl;
        ::kill(getpid(),SIGTERM);
    }
    else
#endif

    char command[1024];
    char basename[1024];

    ASSERT(sizeof(command)-1 > string(cmd).length());

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

template< class APP >
void PipeApplication<APP>::waiting()
{
  Log::status() << "-" << std::endl;
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
