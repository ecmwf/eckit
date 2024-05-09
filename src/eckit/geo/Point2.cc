/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Point2.h"

#include <cmath>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


static const Point2 ZERO;


Point2::value_type Point2::norm() const {
    return distance(ZERO);
}


Point2 Point2::normalize() const {
    const auto l = norm();
    return types::is_approximately_equal(l, 0., EPS) ? ZERO : Point2{X / l, Y / l};
}


Point2 Point2::middle(const Point2& p) const {
    return (*this + p) * 0.5;
}


Point2::value_type Point2::distance(const Point2& p, size_t axis) const {
    return std::abs(x(axis) - p.x(axis));
}


Point2::value_type Point2::distance(const Point2& p) const {
    return std::sqrt(distance2(p));
}


Point2::value_type Point2::distance2(const Point2& p) const {
    return (X - p.X) * (X - p.X) + (Y - p.Y) * (Y - p.Y);
}


bool points_equal(const Point2& a, const Point2& b, Point2::value_type eps) {
    return types::is_approximately_equal(a.X, b.X, eps) && types::is_approximately_equal(a.Y, b.Y, eps);
}


}  // namespace eckit::geo
