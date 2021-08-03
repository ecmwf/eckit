/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ThreadPool.cc
// Baudouin Raoult - (c) ECMWF Feb 12

#include "eckit/thread/ThreadPool.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ThreadPoolThread : public Thread {
    ThreadPool& owner_;
    void run();

public:
    ThreadPoolThread(ThreadPool& owner) :
        owner_(owner) {}
};

void ThreadPoolThread::run() {
    owner_.notifyStart();

    Monitor::instance().name(owner_.name());

    // Log::info() << "Start of ThreadPoolThread " << std::endl;


    for (;;) {
        Monitor::instance().show(false);
        Log::status() << "-" << std::endl;
        ThreadPoolTask* r = owner_.next();
        if (!r)
            break;
        Monitor::instance().show(true);

        r->pool_ = &owner_;


        try {
            r->execute();
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is reported" << std::endl;
            owner_.error(e.what());
        }

        try {
            delete r;
        }
        catch (std::exception& e) {
            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is reported" << std::endl;
            owner_.error(e.what());
        }

        owner_.endTask();
    }


    // Log::info() << "End of ThreadPoolThread " << std::endl;

    owner_.notifyEnd();
}

ThreadPool::ThreadPool(const std::string& name, size_t count, size_t stack) :
    count_(0), stack_(stack), running_(0), tasks_(0), name_(name), error_(false) {
    // Log::info() << "ThreadPool::ThreadPool " << nme_ << " " << count << std::endl;
    resize(count);
}

ThreadPool::~ThreadPool() {
    // Log::info() << "ThreadPool::~ThreadPool " << name_ << std::endl;

    try {
        waitForThreads();
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

void ThreadPool::waitForThreads() {

    for (size_t i = 0; i < count_; i++) {
        push(0);
    }


    AutoLock<MutexCond> lock(done_);
    // Log::info() << "ThreadPool::waitForThreads " << name_ << " running: " << running_ << std::endl;
    while (running_) {
        // Log::info() << "ThreadPool::waitForThreads " << name_ << " running: " << running_ << std::endl;
        done_.wait();
    }

    if (error_) {
        error_ = false;
        throw SeriousBug(std::string("ThreadPool::waitForThreads: ") + errorMessage_);
    }
}

void ThreadPool::notifyStart() {
    AutoLock<MutexCond> lock(done_);
    running_++;
    done_.signal();
    // Log::info() << "ThreadPool::notifyStart " << name_ << " running: " << running_ << std::endl;
}

void ThreadPool::notifyEnd() {
    AutoLock<MutexCond> lock(done_);
    running_--;
    done_.signal();
    // Log::info() << "ThreadPool::notifyEnd " << name_ << " running: " << running_ << std::endl;
}


void ThreadPool::startTask() {
    AutoLock<MutexCond> lock(active_);
    tasks_++;
    active_.signal();
    // Log::info() << "ThreadPool::notifyStart " << name_ << " running: " << running_ << std::endl;
}

void ThreadPool::endTask() {
    AutoLock<MutexCond> lock(active_);
    tasks_--;
    active_.signal();
    // Log::info() << "ThreadPool::notifyStart " << name_ << " running: " << running_ << std::endl;
}

void ThreadPool::error(const std::string& msg) {
    AutoLock<MutexCond> lock(done_);
    if (error_)
        errorMessage_ += " | ";
    error_ = true;
    errorMessage_ += msg;
}

void ThreadPool::push(ThreadPoolTask* r) {
    if (r) {
        startTask();
    }

    AutoLock<MutexCond> lock(ready_);
    queue_.push_back(r);
    ready_.signal();
}

void ThreadPool::push(std::list<ThreadPoolTask*>& l) {
    AutoLock<MutexCond> lock(ready_);

    for (std::list<ThreadPoolTask*>::iterator j = l.begin(); j != l.end(); ++j)
        queue_.push_back((*j));

    l.clear();
    ready_.signal();
}

ThreadPoolTask* ThreadPool::next() {
    AutoLock<MutexCond> lock(ready_);
    while (queue_.empty())
        ready_.wait();

    ThreadPoolTask* r = queue_.front();
    queue_.pop_front();

    if (!queue_.empty())
        ready_.signal();

    return r;
}

void ThreadPool::wait() {
    AutoLock<MutexCond> lock(active_);

    while (tasks_) {
        active_.wait();
    }
}

void ThreadPool::resize(size_t size) {

    while (count_ > size) {
        push(0);
        count_--;
    }

    while (count_ < size) {
        ThreadControler c(new ThreadPoolThread(*this), true, stack_);
        c.start();
        count_++;
    }
}

ThreadPoolTask::~ThreadPoolTask() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
