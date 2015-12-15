/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TelnetUser.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_TelnetUser_h
#define eckit_TelnetUser_h

#include "eckit/net/NetUser.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TelnetUser : public NetUser {
public:
	TelnetUser(TCPSocket&);
	virtual ~TelnetUser();
static void terminate(TelnetUser& other)	{ other.stop(); }

private:
    virtual void serve(Stream&, std::istream&, std::ostream&);
	std::string from_;
};



//-----------------------------------------------------------------------------

} // namespace eckit

#endif
