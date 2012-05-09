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

#ifndef MappedArray_H
#define MappedArray_H

#include "eclib/PathName.h"
#include "eclib/Semaphore.h"

// Used to map an array to a file 

template<class T>
class MappedArray {
public:

	// stl compatibility

	typedef T*       iterator;
	typedef const T* const_iterator;

// -- Contructors

	MappedArray(const PathName&,unsigned long);

// -- Destructor

	~MappedArray(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

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

	MappedArray(const MappedArray<T>&);
	MappedArray<T>& operator=(const MappedArray<T>&);

// -- Members

	Semaphore     sem_;
	void*         map_;
	int           fd_;

	T*            array_;
	unsigned long size_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const MappedArray& p)
	//	{ p.print(s); return s; }

};


#include "MappedArray.cc"

#endif
