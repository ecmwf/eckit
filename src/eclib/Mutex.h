/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Mutex.h
// Baudouin Raoult - ECMWF May 96

#ifndef Mutex_H
#define Mutex_H

#include <pthread.h>

#include "eclib/machine.h"
#include "eclib/NonCopyable.h"

class Mutex : private NonCopyable {
public:

// -- Contructors

	Mutex(char tag = ' ');

// -- Destructor

	~Mutex();

// -- Methods

	void lock();
	void unlock();
	char tag() const { return tag_; }

// -- Class methods

	static Mutex& global();

protected:

// -- Members

	pthread_mutex_t mutex_;
	bool            exists_;
	char            tag_;

};

//-----------------------------------------------------------------

#define CREATE_MUTEX()                              \
static Mutex* mutex;                                \
static pthread_once_t mutex_once = PTHREAD_ONCE_INIT;     \
static void init_global_mutex()                     \
{                                                   \
    mutex = new Mutex();                            \
}

#define INIT_MUTEX() pthread_once(&mutex_once, init_global_mutex);

#endif
