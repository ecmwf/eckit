/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SimpleMappedArray.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef SimpleMappedArray_H
#define SimpleMappedArray_H

#include "eckit/filesystem/PathName.h"

// Used to map an array to a file 

namespace eckit {

template<class T>
class SimpleMappedArray {
public:

	// stl compatibility

	typedef T*       iterator;
	typedef const T* const_iterator;

// -- Contructors

	SimpleMappedArray(const eckit::PathName&,unsigned long = 0);

// -- Destructor

	~SimpleMappedArray(); 

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void sync();

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
	
	// void print(std::ostream&) const; 	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	SimpleMappedArray(const SimpleMappedArray<T>&);
	SimpleMappedArray<T>& operator=(const SimpleMappedArray<T>&);

// -- Members

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

	//friend std::ostream& operator<<(std::ostream& s,const SimpleMappedArray& p)
	//	{ p.print(s); return s; }

};
} //namespace

#include "SimpleMappedArray.cc"

#endif
