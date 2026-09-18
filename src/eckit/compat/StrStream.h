// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
