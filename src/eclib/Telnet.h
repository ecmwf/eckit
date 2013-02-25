/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Telnet.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef eclib_Telnet_h
#define eclib_Telnet_h

#include "eclib/NetService.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class Telnet : public NetService {
public:

// -- Contructors

	Telnet(int);

// -- Destructor

	~Telnet();

private:

// No copy allowed

	Telnet(const Telnet&);
	Telnet& operator=(const Telnet&);

// -- Overridden methods

	// From NetService

	virtual NetUser* newUser(TCPSocket&);
	virtual string   name() { return "telnet"; }

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
