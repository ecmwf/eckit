/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MappedArray.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eckit_MappedArray_h
#define eckit_MappedArray_h

#include "eclib/NonCopyable.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/os/Semaphore.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Used to map an array to a file 

template<class T>
class MappedArray : private NonCopyable {
public:

	// stl compatibility

	typedef T*       iterator;
	typedef const T* const_iterator;

// -- Contructors

	MappedArray(const PathName&,unsigned long);

// -- Destructor

	~MappedArray(); // Change to virtual if base class

// -- Methods

	void sync();
	void lock()    { sem_.lock() ; }
	void unlock()  { sem_.unlock();}

	// stl compatibility

	iterator begin()               { return array_;        }
	iterator end()                 { return array_ + size_; }
	const_iterator begin() const   { return array_;        }
	const_iterator end()   const   { return array_ + size_; }

	unsigned long size()           { return size_;     }
	T& operator[](unsigned long n) { return array_[n]; }

private: // members

	Semaphore     sem_;
	void*         map_;
	int           fd_;

	T*            array_;
	unsigned long size_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#include "MappedArray.cc"

#endif
