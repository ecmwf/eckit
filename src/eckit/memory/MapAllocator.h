/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MapAllocator.h
// MARS (Baudouin Raoult) - ECMWF Nov 01

#ifndef eckit_MapAllocator_h
#define eckit_MapAllocator_h

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MapAllocatorTooSmall : public Exception {
public:	
	MapAllocatorTooSmall(size_t,size_t);
};

class MapAllocator {
public:

// -- Exceptions
	// None

// -- Contructors

	MapAllocator(size_t);

// -- Destructor

	~MapAllocator(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void *allocate(size_t);
	void deallocate(void*);

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

	MapAllocator(const MapAllocator&);
	MapAllocator& operator=(const MapAllocator&);

// -- Members

	int      fd_;
	void    *addr_;
	char    *next_;
	size_t   length_;
	size_t   left_;
	Ordinal  count_;

	MapAllocator* more_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const MapAllocator& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
