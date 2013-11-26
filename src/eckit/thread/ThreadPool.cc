/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ThreadPool.cc
// Baudouin Raoult - (c) ECMWF Feb 12

#include "eckit/thread/AutoLock.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/Thread.h"
#include "eckit/thread/ThreadControler.h"
#include "eckit/thread/ThreadPool.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ThreadPoolThread : public Thread {
    ThreadPool& owner_;
    void run();
    public:
    ThreadPoolThread(ThreadPool& owner): 
        owner_(owner) {}
};

void ThreadPoolThread::run() 
{
    owner_.notifyStart();

    static const char *here = __FUNCTION__;
    Monitor::instance().name(owner_.name());

    //Log::info() << "Start of ThreadPoolThread " << std::endl;


    for(;;)
    {
        Monitor::instance().show(false);
        Log::status() << "-" << std::endl;
        ThreadPoolTask* r = owner_.next();
        if(!r) break;
        Monitor::instance().show(true);

        r->pool_ = &owner_;

        try {
            r->execute();
        }
        catch(std::exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here << std::endl;
            Log::error() << "** Exception is reported" << std::endl;
            owner_.error(e.what());
        }

        try {
            delete r;
        }
        catch(std::exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here << std::endl;
            Log::error() << "** Exception is reported" << std::endl;
            owner_.error(e.what());
        }
    }


    Log::info() << "End of ThreadPoolThread " << std::endl;

    owner_.notifyEnd();
}

ThreadPool::ThreadPool(const string& name,int count, size_t stack):
    name_(name),
    count_(count),
    running_(0),
    error_(false)
{

    //Log::info() << "ThreadPool::ThreadPool " << name_ << " " << count << std::endl;

    for(int i = 0; i < count ; i++)
    {
        ThreadControler c(new ThreadPoolThread(*this), true, stack);
        c.start();
    }
}

ThreadPool::~ThreadPool()
{
    static const char *here = __FUNCTION__;

    //Log::info() << "ThreadPool::~ThreadPool " << name_ << std::endl;

    try {
        waitForThreads();
    }
    catch(std::exception& e)
    {       
        Log::error() << "** " << e.what() << " Caught in " << here << std::endl;
        Log::error() << "** Exception is ignored" << std::endl;
    }
}

void ThreadPool::waitForThreads()
{

    for(int i = 0; i < count_ ; i++)
        push(0);


    AutoLock<MutexCond> lock(done_);
    //Log::info() << "ThreadPool::waitForThreads " << name_ << " running: " << running_ << std::endl;
    while(running_)
	{	
        //Log::info() << "ThreadPool::waitForThreads " << name_ << " running: " << running_ << std::endl;
        done_.wait();
	}

    if(error_)
    {
        error_ = false;
        throw SeriousBug(string("ThreadPool::waitForThreads: ") + errorMessage_);
    }
}

void ThreadPool::notifyStart()
{
    AutoLock<MutexCond> lock(done_);
    running_++;
    done_.signal();
//Log::info() << "ThreadPool::notifyStart " << name_ << " running: " << running_ << std::endl;
}

void ThreadPool::notifyEnd()
{
    AutoLock<MutexCond> lock(done_);
    running_--;
    done_.signal();
//Log::info() << "ThreadPool::notifyEnd " << name_ << " running: " << running_ << std::endl;
}

void ThreadPool::error(const string& msg)
{
    AutoLock<MutexCond> lock(done_);
    if(error_) errorMessage_ += " | ";
    error_ = true;
    errorMessage_ += msg;

}

void ThreadPool::push(ThreadPoolTask* r)
{
    AutoLock<MutexCond> lock(ready_);
    queue_.push_back(r);
    ready_.signal();
}

void ThreadPool::push(std::list<ThreadPoolTask*>& l)
{
    AutoLock<MutexCond> lock(ready_);

    for(std::list<ThreadPoolTask*>::iterator j = l.begin(); j != l.end(); ++j)
        queue_.push_back((*j));

    l.clear();
    ready_.signal();
}

ThreadPoolTask* ThreadPool::next() 
{
    AutoLock<MutexCond> lock(ready_);
    while(queue_.empty())
        ready_.wait();

    ThreadPoolTask* r = queue_.front();
    queue_.pop_front();

    if(!queue_.empty())
        ready_.signal();

    return r;
}


//-----------------------------------------------------------------------------

} // namespace eckit

