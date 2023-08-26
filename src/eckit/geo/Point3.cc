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

#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geo {

//----------------------------------------------------------------------------------------------------------------------

bool points_equal(const Point3& a, const Point3& b) {
    auto eps = 1e-6;
    return types::is_approximately_equal(a.X, b.X, eps) && types::is_approximately_equal(a.Y, b.Y, eps) && types::is_approximately_equal(a.Z, b.Z, eps);
}

bool operator<(const Point3& a, const Point3& b) {
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo
