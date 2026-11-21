// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/OblateSpheroid.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Spec.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::figure {


OblateSpheroid::OblateSpheroid(double a, double b) : a_(a), b_(b) {
    if (!(0. < b && b_ <= a_)) {
        throw BadValue("OblateSpheroid::R requires 0 < b <= a", Here());
    }
}


OblateSpheroid::OblateSpheroid(const Spec& spec) : OblateSpheroid(spec.get_double("a"), spec.get_double("b")) {}


double OblateSpheroid::R() const {
    return R(a_, b_);
}


double OblateSpheroid::R(double a, double b) {
    if (types::is_approximately_equal(a, b)) {
        return a;
    }

    throw BadValue("OblateSpheroid::R requires a ~= b", Here());
}


double OblateSpheroid::eccentricity(double a, double b) {
    ASSERT(0. < b && b <= a);
    return std::sqrt(1. - b * b / (a * a));
}


double OblateSpheroid::flattening(double a, double b) {
    ASSERT(0. < b && b <= a);
    return (a - b) / a;
}


}  // namespace eckit::geo::figure
