/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/log/Log.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/runtime/ProcessControler.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;

ProcessControler::ProcessControler(bool forget) :
    ClassExtent<ProcessControler>(this), pid_(-1), child_(false), status_(0), forget_(forget) {}

ProcessControler::~ProcessControler() {
    if (!forget_ && active())
        Log::warning() << "~ProcessControler called while process still active" << std::endl;
}

void ProcessControler::printStatus(pid_t pid, int status) {
    Log::info() << "-------- End of " << pid;

    if (WIFEXITED(status))
        Log::info() << " exited ";

    if (WEXITSTATUS(status))
        Log::info() << " status " << WEXITSTATUS(status) << ' ';

    if (WIFSIGNALED(status))
        Log::info() << " with signal " << WTERMSIG(status);

    Log::info() << std::endl;
}

void ProcessControler::end(result& r) {
    if (r.pid_ == pid_) {
        r.found_ = true;
        status_  = r.status_;
        pid_     = -pid_;
    }
}

// This thread should receive the signals

class ChildReaper : public Thread {
    virtual void run();
};

void ChildReaper::run() {

    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);
    pthread_sigmask(SIG_UNBLOCK, &newmask, &oldmask);

    Monitor::instance().name("reaper");

    for (;;) {
        ProcessControler::result r;
        r.found_ = false;
        Log::status() << "Waiting" << std::endl;

        Monitor::instance().show(false);
        r.pid_ = ::waitpid(-1, &r.status_, 0);
        Monitor::instance().show(true);

        if (r.pid_ == -1) {
            // Todo: use mutex cond....
            if (errno != ECHILD)
                Log::error() << "Wait pid " << Log::syserr << std::endl;
            ::sleep(5);
        }
        else {
            Log::status() << "End of " << r.pid_ << std::endl;
            ProcessControler::printStatus(r.pid_, r.status_);
            ProcessControler::callAll(&ProcessControler::end, r);
        }
    }
}

static void init() {
    ThreadControler ctl(new ChildReaper());
    ctl.start();

    /*
        // For now, ignore the sig childs...
        // should use sigaction instead of signal
        signal(SIGCHLD, SIG_IGN);
    */
}

void ProcessControler::start() {
    pthread_once(&once, init);

    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);
    pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);

    switch (pid_ = ::fork()) {
        case 0:

            pid_   = ::getpid();
            child_ = true;

            // Log::info() << "Starting new process with a pid of " << pid_ << std::endl;

            try {
                afterForkInChild();
                run();
            }
            catch (std::exception& e) {

                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is terminate process " << pid_ << std::endl;
            }


            ::exit(0);
            break;

        case -1:
            Log::error() << "Cannot fork " << Log::syserr << std::endl;
            throw FailedSystemCall("fork");

        default:
            break;
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);
    pthread_sigmask(SIG_UNBLOCK, &newmask, &oldmask);

    afterForkInParent();
}

void ProcessControler::stop() {
    if (!active())
        return;

    Log::info() << "ProcessControler::stop " << child_ << '-' << pid_ << std::endl;
    if (child_)
        ::exit(0);
    else
        ::kill(pid_, SIGTERM);

    // pid_ = -1;
}

void ProcessControler::kill() {
    stop();  // For the time being....
}

void ProcessControler::wait() {
    int status;

    if (!active())
        return;

    Log::info() << "ProcessControler::wait " << pid_ << " " << child_ << std::endl;
    if (pid_ != -1 && !child_) {
        pid_t pid = ::waitpid(pid_, &status, 0);

        if (pid != pid_) {
            Log::error() << "Wait pid returns " << errno << ' ' << pid << std::endl;
            Log::error() << Log::syserr << std::endl;
        }
        else {

            printStatus(pid, status);

            pid_    = -1;
            status_ = status;
        }
    }
}

bool ProcessControler::active() {
    if (pid_ < 0 && pid_ != -1) {
        printStatus(-pid_, status_);
        pid_ = -1;
    }
    return pid_ != -1;
}


bool ProcessControler::isRunning(pid_t pid) {
#ifdef IBM
    pid_t p = ::getsid(pid);
    if (p == -1) {
        if (errno == EPERM)
            return true;

        if (errno == ESRCH)
            return false;

        throw FailedSystemCall("getsid");
    }
    return true;
#else
#ifdef __APPLE__
    return ::kill(pid, 0) == 0;
#else
    char buf[1024];
    snprintf(buf, 1024, "/proc/%d", pid);
    return access(buf, F_OK) == 0;
#endif

#endif
}

void ProcessControler::afterForkInParent() {}

void ProcessControler::afterForkInChild() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
