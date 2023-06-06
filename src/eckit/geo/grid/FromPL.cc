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


#include "eckit/geo/grid/FromPL.h"


namespace eckit::geo::grid {


FromPL::FromPL(const Configuration& config) :
    Reduced(config) {}


FromPL::FromPL(size_t N, const std::vector<long>& pl, const BoundingBox& bbox, double angularPrecision) :
    Reduced(N, pl, bbox, angularPrecision) {}


bool FromPL::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const FromPL*>(&other);
    return (o != nullptr) && (pls() == o->pls()) && Reduced::sameAs(other);
}


}  // namespace eckit::geo::grid
