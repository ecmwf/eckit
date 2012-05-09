/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Thread.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef Thread_H
#define Thread_H

#include "eclib/NonCopyable.h"
#include "eclib/Mutex.h"

// This should be a Task

class Thread : public NonCopyable<Thread> {
public:
	friend class ThreadControler;

// -- Contructors
	
	Thread(bool autodel = true); // 

// -- Destructor

	virtual ~Thread();

// -- Methods

	void stop();

protected:

// -- Members

	Mutex mutex_;

// -- Methods

	bool  stopped();

private:

// -- Members

	bool    stop_;
	bool    autodel_;
#ifdef linux
	void*   data_;
#endif

// -- Methods
	
	virtual void run() = 0;

};

#endif
