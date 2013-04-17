/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Statistics.h
// Baudouin Raoult - ECMWF Sep 97

#ifndef eckit_Statistics_h
#define eckit_Statistics_h

#include "eckit/types/Types.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Statistics {
public:

// -- Exceptions
	// None

// -- Contructors

	Statistics();

// -- Destructor

	~Statistics(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods

	static void readFile(const MarsID&,const PathName&,const Length&);
	static void createFile(const MarsID&,const PathName&,const Length&,const string&);
	static void deleteFile(const PathName&);


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

	Statistics(const Statistics&);
	Statistics& operator=(const Statistics&);

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

	//friend ostream& operator<<(ostream& s,const Statistics& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit


#endif
