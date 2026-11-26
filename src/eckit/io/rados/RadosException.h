// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date March 2024

#pragma once

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosException : public eckit::Exception {
public:

    RadosException(const std::string& msg);
    RadosException(const std::string& msg, const eckit::CodeLocation& loc);
    RadosException(const std::string& msg, const eckit::CodeLocation& loc, bool quiet);
};

//----------------------------------------------------------------------------------------------------------------------

/// An object, key-value, key or pool does not exist.
/// @note: this is an ordinary outcome which callers handle, so, unlike other exceptions, it is
///   not logged as an error when thrown.
class RadosEntityNotFoundException : public RadosException {
public:

    RadosEntityNotFoundException(const std::string& msg);
    RadosEntityNotFoundException(const std::string& msg, const eckit::CodeLocation& loc);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
