/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/PointXY.h"

#include <cmath>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


static const PointXY ZERO;


PointXY::value_type PointXY::norm() const {
    return distance(ZERO);
}


PointXY PointXY::normalize() const {
    const auto l = norm();
    return types::is_approximately_equal(l, 0., EPS) ? ZERO : PointXY{X / l, Y / l};
}


PointXY PointXY::middle(const PointXY& p) const {
    return (*this + p) * 0.5;
}


PointXY::value_type PointXY::distance(const PointXY& p, size_t axis) const {
    return std::abs(x(axis) - p.x(axis));
}


PointXY::value_type PointXY::distance(const PointXY& p) const {
    return std::sqrt(distance2(p));
}


PointXY::value_type PointXY::distance2(const PointXY& p) const {
    return (X - p.X) * (X - p.X) + (Y - p.Y) * (Y - p.Y);
}


bool points_equal(const PointXY& a, const PointXY& b, PointXY::value_type eps) {
    return types::is_approximately_equal(a.X, b.X, eps) && types::is_approximately_equal(a.Y, b.Y, eps);
}


}  // namespace eckit::geo
