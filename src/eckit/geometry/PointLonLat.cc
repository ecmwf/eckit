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

#include "eckit/geometry/PointLonLat.h"

#include "eckit/geometry/UnitSphere.h"
#include "eckit/types/FloatCompare.h"

namespace eckit::geometry {

bool points_equal(const PointLonLat& a, const PointLonLat& b) {
    return eckit::types::is_approximately_equal<double>(UnitSphere::centralAngle(a, b), 0.0);
}

}  // namespace eckit::geometry
