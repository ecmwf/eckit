/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
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
