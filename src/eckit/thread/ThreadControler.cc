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

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


ThreadControler::ThreadControler(Thread* proc, bool detached, size_t stack) :
    detached_(detached), thread_(0), proc_(proc), stack_(stack), running_(false) {
    // cout << "ThreadControler::ThreadControler(" << this << ")" << " " << hex << pthread_self() << std::endl;
}

ThreadControler::~ThreadControler() {
    // cout << "ThreadControler::~ThreadControler(" << this << ")" << " " <<hex <<  pthread_self() << std::endl;
    AutoLock<MutexCond> lock(cond_);

    if (running_) {
        // The Thread will delete itself
        // so there is no need for:
        // delete proc_;
    }
    else {
        // Log::warning() << "Deleting Thread in ThreadControler::~ThreadControler()" << " " << hex << pthread_self() <<
        // std::endl;
        delete proc_;
        proc_ = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void ThreadControler::execute() {
    // Make a copy, because "this" will desappear
    Thread* proc = proc_;
    if (detached_)
        proc_ = nullptr;

    // cout << "ThreadControler::execute(" << this << ")" <<  " " << hex << pthread_self() << std::endl;
    //=================
    // Make sure the logs are created...

    Monitor::instance().startup();
    Monitor::instance().parent(Main::instance().taskID());

    //============


    {  // Signal that we are running

        AutoLock<MutexCond> lock(cond_);
        running_ = true;
        cond_.signal();

        // NOTE: "this" is not valid any more after this point
    }

    //=============

    // We don't want to receive reconfigure events

    sigset_t set, old_set;

    sigemptyset(&set);

    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGPIPE);

    THRCALL(::pthread_sigmask(SIG_BLOCK, &set, &old_set));

    //=============

    ASSERT(proc);

    try {
        proc->run();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception terminates thread " << pthread_self() << std::endl;
    }

    if (proc->autodel_) {
        delete proc;
    }
}

void* ThreadControler::startThread(void* data) {
    // cout << "ThreadControler::startThread(" << data << ")" << " " << hex << pthread_self() << std::endl;
    reinterpret_cast<ThreadControler*>(data)->execute();  // static_cast or dynamic_cast ??
    return 0;
}

void ThreadControler::start() {
    // cout << "ThreadControler::start(" << this << ")" << " " << hex << pthread_self() << std::endl;
    ASSERT(thread_ == 0);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (stack_) {
        THRCALL(::pthread_attr_setstacksize(&attr, stack_));
    }
    if (detached_)
        THRCALL(::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
    else
        THRCALL(::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));

    AutoLock<MutexCond> lock(cond_);

    THRCALL(::pthread_create(&thread_, &attr, startThread, this));

    pthread_attr_destroy(&attr);

    while (!running_)
        cond_.wait();
}

void ThreadControler::kill() {
    pthread_cancel(thread_);
    // pthread_kill(thread_,sig);
}

void ThreadControler::stop() {
    // Due to legacy code, this stop routine may be called on detached threads. Don't
    // stress about it!
    if (!detached_ && proc_ != NULL) {
        proc_->stop();
    }
}

void ThreadControler::wait() {
    ASSERT(!detached_);
    // if(running_)
    THRCALL(::pthread_join(thread_, 0));
}

bool ThreadControler::active() {
    if (thread_ != 0) {
        // Try see if it exists

        int policy;
        sched_param param;

        int n = pthread_getschedparam(thread_, &policy, &param);

        // The thread does not exist
        if (n != 0)
            thread_ = 0;
    }
    return thread_ != 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
