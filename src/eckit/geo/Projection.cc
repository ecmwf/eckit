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


#include "eckit/geo/Projection.h"

#include <sstream>


namespace eckit::geo {


ProjectionProblem::ProjectionProblem(const std::string& what, const CodeLocation& loc) : Exception(loc) {
    std::ostringstream s;
    s << "ProjectionProblem: [" << what << "], in " << loc;
    reason(s.str());
};


}  // namespace eckit::geo
