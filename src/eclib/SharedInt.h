/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SharedInt.h
// Baudouin Raoult - ECMWF Jul 97

#ifndef eclib_SharedInt_h
#define eclib_SharedInt_h

#include "eclib/Semaphore.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class SharedInt : public Semaphore {
public:

// -- Exceptions
	// None

// -- Contructors

	SharedInt(const PathName&,int n = 1);

// -- Destructor

	~SharedInt(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void use(int n = 0);
	void release(int n = 0);

	int  free(int n = 0) const;
	int  limit(int n = 0)   const;
	void newLimit(int val,int n = 0);

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

	SharedInt(const SharedInt&);
	SharedInt& operator=(const SharedInt&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const SharedInt& p)
	//	{ p.print(s); return s; }

};



//-----------------------------------------------------------------------------

} // namespace eclib

#endif
