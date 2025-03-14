/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StrStream.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_StrStream_h
#define eckit_StrStream_h

#include <iosfwd>
#include <sstream>

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class StrStream : public std::ostringstream {
public:

    operator std::string() { return str(); }
    static std::ostream& ends(std::ostream& os) { return os; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
