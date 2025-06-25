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


AreaError::AreaError(const std::string& what, const CodeLocation& location) :
    Exception("AreaError: [" + what + "]", location) {}


OrderError::OrderError(const std::string& what, const CodeLocation& location) :
    Exception("OrderError: [" + what + "]", location) {}


ProjectionError::ProjectionError(const std::string& what, const CodeLocation& location) :
    Exception("ProjectionError: [" + what + "]", location) {}


SpecError::SpecError(const std::string& what, const CodeLocation& location) :
    Exception("SpecError: [" + what + "]", location) {}


}  // namespace eckit::geo::exception
