/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ThreadPool_h
#define eckit_ThreadPool_h

#include "eclib/machine.h"
#include "eckit/thread/MutexCond.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ThreadPool;

class ThreadPoolTask {
    ThreadPool* pool_;
protected:
    ThreadPool& pool() { return *pool_; }
public:
    virtual ~ThreadPoolTask() {}
    virtual void execute() = 0;

    friend class ThreadPoolThread;
};

class ThreadPool : private NonCopyable {
public:

    ThreadPool(const string& name, int count, size_t stack = 0);

	~ThreadPool();
    
// -- Methods

    void push(ThreadPoolTask*);
    void push(list<ThreadPoolTask*>&);
    ThreadPoolTask* next();
    void notifyStart();
    void notifyEnd();
    void waitForThreads();
    const string& name() const { return name_; }
    void error(const string&);

private: // members

    MutexCond ready_;
    MutexCond done_;
    int  count_;
    int  running_;
    bool error_;
    string errorMessage_;
    string name_;
    list<ThreadPoolTask*> queue_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
