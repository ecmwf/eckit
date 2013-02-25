/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ThreadControler.h
// Baudouin Raoult - ECMWF May 96

#ifndef eclib_ThreadControler_h
#define eclib_ThreadControler_h

#include "eclib/MutexCond.h"
#include "eclib/Task.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------


class Thread;

/// @note Don't subclass from ThreadControler but from Thread
class ThreadControler : public Task {
public:

// -- Contructors
	
	/// @note ThreadControler takes ownership of Thread
    ThreadControler(Thread*,bool detached = true, size_t stack = 0);

// -- Destructor
	
	~ThreadControler();

// -- Overridden methods

	// From Task

	virtual void start();
	virtual void stop();
	virtual void kill();
	virtual void wait();
	virtual bool active();

protected:

// -- Members
	
	MutexCond  cond_;
	bool       detached_;

private:

// -- Members
	
    pthread_t   thread_;
	Thread     *proc_;
    bool        running_;
    size_t      stack_;

// -- Methods
	
	void execute();

    static void* startThread (void *data);

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
