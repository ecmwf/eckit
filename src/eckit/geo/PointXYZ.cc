/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/PointXYZ.h"

#include <cmath>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointXYZ::value_type PointXYZ::distance(const PointXYZ& p, size_t axis) const {
    return std::abs(x(axis) - p.x(axis));
}


PointXYZ::value_type PointXYZ::distance(const PointXYZ& p) const {
    return std::sqrt(distance2(p));
}


PointXYZ::value_type PointXYZ::distance2(const PointXYZ& p) const {
    return (X - p.X) * (X - p.X) + (Y - p.Y) * (Y - p.Y) + (Z - p.Z) * (Z - p.Z);
}


bool points_equal(const PointXYZ& a, const PointXYZ& b, PointXYZ::value_type eps) {
    return types::is_approximately_equal(a.X, b.X, eps) && types::is_approximately_equal(a.Y, b.Y, eps) &&
           types::is_approximately_equal(a.Z, b.Z, eps);
}


}  // namespace eckit::geo
