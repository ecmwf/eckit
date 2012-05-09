/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ThreadPool.cc
// Baudouin Raoult - (c) ECMWF Feb 12


#include "eclib/AutoLock.h"
#include "eclib/Monitor.h"
#include "eclib/Thread.h"
#include "eclib/ThreadControler.h"
#include "eclib/ThreadPool.h"

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
    Monitor::name(owner_.name());

    Log::info() << "Start of ThreadPoolThread " << endl;


    for(;;)
    {
        Log::status() << "-" << endl;
        ThreadPoolTask* r = owner_.next();
        if(!r) break;

        try {
            r->execute();
        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
            Log::error() << "** Exception is reported" << endl;
            owner_.error(e.what());
        }

        try {
            delete r;
        }
        catch(exception& e)
        {
            Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
            Log::error() << "** Exception is reported" << endl;
            owner_.error(e.what());
        }
    }


    Log::info() << "End of ThreadPoolThread " << endl;

    owner_.notifyEnd();
}

ThreadPool::ThreadPool(const string& name,int count):
    name_(name),
    count_(count),
    running_(0),
    error_(false)
{
    for(int i = 0; i < count ; i++)
    {
        ThreadControler c(new ThreadPoolThread(*this));
        c.start();
    }
}

ThreadPool::~ThreadPool()
{
    static const char *here = __FUNCTION__;

    try {
        waitForThreads();
    }
    catch(exception& e)
    {       
        Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
        Log::error() << "** Exception is ignored" << endl;
    }
}

void ThreadPool::waitForThreads()
{

    for(int i = 0; i < count_ ; i++)
        push(0);


    AutoLock<MutexCond> lock(done_);
    while(running_)
        done_.wait();

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
}

void ThreadPool::notifyEnd()
{
    AutoLock<MutexCond> lock(done_);
    running_--;
    done_.signal();
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

void ThreadPool::push(list<ThreadPoolTask*>& l)
{
    AutoLock<MutexCond> lock(ready_);

    for(list<ThreadPoolTask*>::iterator j = l.begin(); j != l.end(); ++j)
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

