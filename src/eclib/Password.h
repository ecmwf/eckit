/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Password.h
// Baudouin Raoult - ECMWF Dec 97

#ifndef eckit_Password_h
#define eckit_Password_h

#include "eclib/machine.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Password {
public:

	static bool check(const string&,const string&);
	static string salt(const string&);

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
