/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Exceptions.h"


namespace eckit::geo::exception {


ProjectionError::ProjectionError(const std::string& what, const CodeLocation& loc) : Exception(loc) {
    reason("ProjectionError: [" + what + "], in " + loc.asString());
};


SpecNotFound::SpecNotFound(const std::string& what, const CodeLocation& loc) : Exception(loc) {
    reason("SpecNotFound: [" + what + "], in " + loc.asString());
};


}  // namespace eckit::geo::exception
