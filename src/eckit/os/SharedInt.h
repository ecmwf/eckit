/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SharedInt.h
// Baudouin Raoult - ECMWF Jul 97

#ifndef eckit_SharedInt_h
#define eckit_SharedInt_h

#include "eckit/os/Semaphore.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class SharedInt : public Semaphore {
public:

// -- Exceptions
	// None

// -- Contructors

	SharedInt(const PathName&,int n = 1);

// -- Destructor

	~SharedInt();

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void use(int n = 0);
	void release(int n = 0);

	int  free(int n = 0) const;
	int  limit(int n = 0)   const;
    void newLimit(short val,unsigned short n = 0);

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

	//friend std::ostream& operator<<(std::ostream& s,const SharedInt& p)
	//	{ p.print(s); return s; }

};



//-----------------------------------------------------------------------------

} // namespace eckit

#endif
