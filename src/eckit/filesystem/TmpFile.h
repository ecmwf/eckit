/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TmpFile.h
// Baudouin Raoult - ECMWF Sep 01

#ifndef eckit_TmpFile_h
#define eckit_TmpFile_h

#include "eckit/filesystem/PathName.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TmpFile : public PathName {
public:

// -- Exceptions
	// None

// -- Contructors

	TmpFile();

// -- Destructor

	~TmpFile(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators


// -- Methods
	// None

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

	TmpFile(const TmpFile&);
	TmpFile& operator=(const TmpFile&);

// -- Members


// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const TmpFile& p)
	//	{ p.print(s); return s; }

};



//-----------------------------------------------------------------------------

} // namespace eckit


#endif
