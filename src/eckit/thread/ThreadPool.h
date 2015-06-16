/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ThreadPool_h
#define eckit_ThreadPool_h

#include "eckit/eckit.h"
#include "eckit/thread/MutexCond.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class ThreadPool;

//-----------------------------------------------------------------------------

class ThreadPoolTask {
public:

    virtual ~ThreadPoolTask() {}
    virtual void execute() = 0;

    friend class ThreadPoolThread;

protected:

    ThreadPool& pool() { return *pool_; }

private:

    ThreadPool* pool_;

};

//-----------------------------------------------------------------------------

class ThreadPool : private NonCopyable {

public: // methods

    ThreadPool(const std::string& name, int count, size_t stack = 0);

	~ThreadPool();
    
    void push(ThreadPoolTask*);
    void push(std::list<ThreadPoolTask*>&);
    ThreadPoolTask* next();
    void notifyStart();
    void notifyEnd();
    void waitForThreads();
    const std::string& name() const { return name_; }
    void error(const std::string&);

private: // members

    MutexCond ready_;
    MutexCond done_;

    int  count_;
    int  running_;

    std::string errorMessage_;
    std::string name_;
    std::list<ThreadPoolTask*> queue_;

    bool error_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
