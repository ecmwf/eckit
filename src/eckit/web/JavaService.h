/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JavaService.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef JavaService_H
#define JavaService_H

#include "eckit/net/NetService.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class JavaService : public eckit::NetService {
public:

// -- Exceptions
	// None

// -- Contructors

	JavaService(int port);

// -- Destructor

    ~JavaService();

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
	// None

protected:

// -- Members
	// None

// -- Methods
	
    // void print(ostream&) const;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	JavaService(const JavaService&);
	JavaService& operator=(const JavaService&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods

	virtual eckit::NetUser* newUser(eckit::TCPSocket& socket);
	virtual string name() { return "java"; }

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const JavaService& p)
	//	{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
