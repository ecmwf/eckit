// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Password.h
// Baudouin Raoult - ECMWF Dec 97

#ifndef eckit_Password_h
#define eckit_Password_h

#include <string>

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Password {
public:

    static bool check(const std::string&, const std::string&);
    static std::string salt(const std::string&);
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
