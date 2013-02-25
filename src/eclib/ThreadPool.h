/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ThreadPool.h
// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef eclib_ThreadPool_h
#define eclib_ThreadPool_h

#include "eclib/machine.h"
#include "eclib/MutexCond.h"

//-----------------------------------------------------------------------------

namespace eclib {

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

class ThreadPool {
public:

// -- Exceptions
	// None

// -- Contructors

    ThreadPool(const string& name, int count, size_t stack = 0);

// -- Destructor

	~ThreadPool(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    void push(ThreadPoolTask*);
    void push(list<ThreadPoolTask*>&);
    ThreadPoolTask* next();
    void notifyStart();
    void notifyEnd();
    void waitForThreads();
    const string& name() const { return name_; }
    void error(const string&);


// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods

	// void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	ThreadPool(const ThreadPool&);
	ThreadPool& operator=(const ThreadPool&);

// -- Members

    MutexCond ready_;
    MutexCond done_;
    int  count_;
    int  running_;
    bool error_;
    string errorMessage_;
    string name_;
    list<ThreadPoolTask*> queue_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const ThreadPool& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
