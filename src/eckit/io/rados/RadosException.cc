// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosException.h"

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosException::RadosException(const std::string& w, const eckit::CodeLocation& l, int code) :
    Exception(w, l), code_(code) {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit