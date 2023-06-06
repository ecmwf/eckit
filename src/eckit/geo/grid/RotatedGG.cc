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


#include "eckit/geo/grid/RotatedGG.h"

#include <ostream>
#include <utility>

#include "eckit/geo/grid/GaussianIterator.h"


namespace eckit::geo::grid {


RotatedGG::RotatedGG(const Configuration& config) :
    Regular(config), rotation_(parametrisation) {}


RotatedGG::RotatedGG(size_t N, const Rotation& rotation, const BoundingBox& bbox, double angularPrecision) :
    Regular(N, bbox, angularPrecision), rotation_(rotation) {}


void RotatedGG::print(std::ostream& out) const {
    out << "RotatedGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox() << ",rotation=" << rotation_ << "]";
}


bool RotatedGG::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const RotatedGG*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && Regular::sameAs(other);
}


Iterator* RotatedGG::iterator() const {
    std::vector<long> pl(N_ * 2, long(4 * N_));
    return new GaussianIterator(latitudes(), std::move(pl), bbox(), N_, Nj_, k_, rotation_);
}


const Grid* RotatedGG::croppedGrid(const BoundingBox& bbox) const {
    return new RotatedGG(N_, rotation_, bbox, angularPrecision_);
}


static const GridBuilder<RotatedGG> rotatedGG("rotated_gg");


}  // namespace eckit::geo::grid
