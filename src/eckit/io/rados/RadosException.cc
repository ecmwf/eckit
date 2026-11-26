/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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