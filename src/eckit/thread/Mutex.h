/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Mutex.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Mutex_h
#define eckit_Mutex_h

#include <pthread.h>

#include "eckit/eckit.h"
#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

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

protected:

// -- Members

	pthread_mutex_t mutex_;
	bool            exists_;
	char            tag_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
