// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_ThreadPool_h
#define eckit_ThreadPool_h

#include <list>
#include <string>

#include "eckit/thread/MutexCond.h"


namespace eckit {

//-----------------------------------------------------------------------------

class ThreadPool;


class ThreadPoolTask {
public:

    virtual ~ThreadPoolTask();
    virtual void execute() = 0;

    friend class ThreadPoolThread;

protected:

    ThreadPool& pool() { return *pool_; }

private:

    ThreadPool* pool_;
};

//-----------------------------------------------------------------------------

class ThreadPool {

public:  // methods

    ThreadPool(const std::string& name, size_t count, size_t stack = 0);

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    ~ThreadPool();

    void push(ThreadPoolTask*);
    void push(std::list<ThreadPoolTask*>&);
    ThreadPoolTask* next();
    void notifyStart();
    void notifyEnd();
    void waitForThreads();
    const std::string& name() const { return name_; }
    void error(const std::string&);

    void wait();
    bool done();
    void resize(size_t);

    void startTask();
    void endTask();

private:  // members

    MutexCond ready_;
    MutexCond done_;
    MutexCond active_;

    size_t count_;
    size_t stack_;
    size_t running_;
    size_t tasks_;

    std::string errorMessage_;
    std::string name_;
    std::list<ThreadPoolTask*> queue_;

    bool error_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
