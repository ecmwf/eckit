/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StrStream.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eclib_StrStream_h
#define eclib_StrStream_h

#include "eclib/machine.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------


class StrStream : public ostringstream {
public:
    operator string() { return str(); }
    static ostream&  ends(ostream& os)  { return os; }
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
