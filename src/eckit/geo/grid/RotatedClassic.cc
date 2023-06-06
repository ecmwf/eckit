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


#include "eckit/geo/grid/RotatedClassic.h"

#include <ostream>


namespace eckit::geo::grid {


RotatedClassic::RotatedClassic(size_t N, const Rotation& rotation, const BoundingBox& bbox,
                               double angularPrecision) :
    Classic(N, bbox, angularPrecision), rotation_(rotation) {}


void RotatedClassic::print(std::ostream& out) const {
    out << "RotatedClassic["
           "N="
        << N_ << ",bbox=" << bbox() << ",rotation=" << rotation_ << "]";
}


bool RotatedClassic::sameAs(const Grid& /*unused*/) const {
    NOTIMP;
}


Iterator* RotatedClassic::iterator() const {
    return rotatedIterator(rotation_);
}


const Grid* RotatedClassic::croppedGrid(const BoundingBox& bbox) const {
    return new RotatedClassic(N_, rotation_, bbox, angularPrecision_);
}


}  // namespace eckit::geo::grid
