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


#include "eckit/geo/grid/ReducedFromPL.h"

#include <ostream>


namespace eckit::geo::grid {


ReducedFromPL::ReducedFromPL(const Configuration& config) :
    FromPL(config) {}


ReducedFromPL::ReducedFromPL(size_t N, const std::vector<long>& pl, const BoundingBox& bbox,
                             double angularPrecision) :
    FromPL(N, pl, bbox, angularPrecision) {}


void ReducedFromPL::print(std::ostream& out) const {
    out << "ReducedFromPL[N=" << N_ << ",bbox=" << bbox() << "]";
}


Iterator* ReducedFromPL::iterator() const {
    return unrotatedIterator();
}


const Grid* ReducedFromPL::croppedGrid(const BoundingBox& bbox) const {
    return new ReducedFromPL(N_, pls(), bbox, angularPrecision_);
}


bool ReducedFromPL::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const ReducedFromPL*>(&other);
    return (o != nullptr) && FromPL::sameAs(other);
}


static const GridBuilder<ReducedFromPL> reducedFromPL("reduced_gg");


}  // namespace eckit::geo::grid
