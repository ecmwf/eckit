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


#include "eckit/geo/figure/OblateSpheroid.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::figure {


OblateSpheroid::OblateSpheroid(double a, double b) : a_(a), b_(b) {
    ASSERT_MSG(types::is_strictly_greater(b_, 0.) && b_ <= a_, "OblateSpheroid requires 0 < b <= a");
}


OblateSpheroid::OblateSpheroid(const Spec& spec) : OblateSpheroid(spec.get_double("a"), spec.get_double("b")) {}


double OblateSpheroid::R() const {
    ASSERT_MSG(types::is_approximately_equal(a_, b_), "OblateSpheroid::R requires a ~= b");
    return a_;
}


double OblateSpheroid::area() const {
    auto e = eccentricity();
    if (types::is_approximately_equal(e, 1.)) {
        return 2. * M_PI * a_ * a_;
    }

    ASSERT(types::is_strictly_greater(e, 0.));
    return 2. * M_PI * a_ * a_ * (1. + (1 - e * e) / e * std::atan2(e, 1 - e * e));
}


}  // namespace eckit::geo::figure
