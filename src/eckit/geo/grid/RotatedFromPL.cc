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


#include "eckit/geo/grid/RotatedFromPL.h"

#include <ostream>


namespace eckit::geo::grid {


RotatedFromPL::RotatedFromPL(const Configuration& config) :
    FromPL(config), rotation_(parametrisation) {}


RotatedFromPL::RotatedFromPL(size_t N, const std::vector<long>& pl, const Rotation& rotation,
                             const BoundingBox& bbox, double angularPrecision) :
    FromPL(N, pl, bbox, angularPrecision), rotation_(rotation) {}


void RotatedFromPL::print(std::ostream& out) const {
    out << "RotatedFromPL["
           "N="
        << N_ << ",bbox=" << bbox() << ",rotation=" << rotation_ << "]";
}


bool RotatedFromPL::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const RotatedFromPL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && FromPL::sameAs(other);
}


Iterator* RotatedFromPL::iterator() const {
    return rotatedIterator(rotation_);
}


const Grid* RotatedFromPL::croppedGrid(const BoundingBox& bbox) const {
    return new RotatedFromPL(N_, pls(), rotation_, bbox, angularPrecision_);
}


static const GridBuilder<RotatedFromPL> rotatedFromPL("reduced_rotated_gg");


}  // namespace eckit::geo::grid
