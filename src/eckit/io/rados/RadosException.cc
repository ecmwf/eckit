// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosException.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosException::RadosException(const std::string& w) : Exception(w) {}

RadosException::RadosException(const std::string& w, const eckit::CodeLocation& l) : Exception(w, l) {}

RadosException::RadosException(const std::string& w, const eckit::CodeLocation& l, bool quiet) :
    Exception(w, l, quiet) {}

RadosEntityNotFoundException::RadosEntityNotFoundException(const std::string& w) :
    RadosException(w, eckit::CodeLocation(), true) {}

RadosEntityNotFoundException::RadosEntityNotFoundException(const std::string& w, const eckit::CodeLocation& l) :
    RadosException(w, l, true) {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit