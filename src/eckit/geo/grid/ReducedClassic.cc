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


#include "eckit/geo/grid/ReducedClassic.h"

#include <ostream>


namespace eckit::geo::grid {


ReducedClassic::ReducedClassic(size_t N, const BoundingBox& bbox, double angularPrecision) :
    Classic(N, bbox, angularPrecision) {}


void ReducedClassic::print(std::ostream& out) const {
    out << "ReducedClassic[N=" << N_ << ",bbox=" << bbox() << "]";
}


Iterator* ReducedClassic::iterator() const {
    return unrotatedIterator();
}


const Grid* ReducedClassic::croppedGrid(const BoundingBox& bbox) const {
    return new ReducedClassic(N_, bbox, angularPrecision_);
}


bool ReducedClassic::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const ReducedClassic*>(&other);
    return (o != nullptr) && Classic::sameAs(other);
}


}  // namespace eckit::geo::grid
