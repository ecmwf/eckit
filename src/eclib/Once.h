/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Once.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef Once_H
#define Once_H

#include <pthread.h>

#include "eclib/NonCopyable.h"

template<class T> class Once : private NonCopyable {
public:

// -- Contructors

	Once();

// -- Destructor

	~Once();

// -- Class methods

	operator T&();

private:

// -- Members
    T* value_;

// -- Class members

	static pthread_once_t once_;
	static pthread_mutex_t mutex_;

// -- Class methods

	static void init(void);

};


#include "eclib/Once.cc"

#endif
