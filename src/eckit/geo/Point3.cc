/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Point3.h"

#include <cmath>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


Point3::value_type Point3::distance(const Point3& p, size_t axis) const {
    return std::abs(x(axis) - p.x(axis));
}


Point3::value_type Point3::distance(const Point3& p) const {
    return std::sqrt(distance2(p));
}


Point3::value_type Point3::distance2(const Point3& p) const {
    return (X - p.X) * (X - p.X) + (Y - p.Y) * (Y - p.Y) + (Z - p.Z) * (Z - p.Z);
}


bool points_equal(const Point3& a, const Point3& b, Point3::value_type eps) {
    return types::is_approximately_equal(a.X, b.X, eps) && types::is_approximately_equal(a.Y, b.Y, eps)
           && types::is_approximately_equal(a.Z, b.Z, eps);
}


}  // namespace eckit::geo
