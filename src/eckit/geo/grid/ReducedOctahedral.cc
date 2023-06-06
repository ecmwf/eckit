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


#include "eckit/geo/grid/ReducedOctahedral.h"

#include <ostream>


namespace eckit::geo::grid {


ReducedOctahedral::ReducedOctahedral(size_t N, const BoundingBox& bbox, double angularPrecision) :
    Octahedral(N, bbox, angularPrecision) {}


void ReducedOctahedral::print(std::ostream& out) const {
    out << "ReducedOctahedral[N=" << N_ << ",bbox=" << bbox() << "]";
}


Iterator* ReducedOctahedral::iterator() const {
    return unrotatedIterator();
}


const Grid* ReducedOctahedral::croppedGrid(const BoundingBox& bbox) const {
    return new ReducedOctahedral(N_, bbox, angularPrecision_);
}


bool ReducedOctahedral::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const ReducedOctahedral*>(&other);
    return (o != nullptr) && Octahedral::sameAs(other);
}


}  // namespace eckit::geo::grid
