// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/OblateSpheroid.h"

#include <cmath>
#include <cstdlib>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
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


PointXYZ OblateSpheroid::convertSphericalToCartesian(double a, double b, const PointLonLat& P, double height) {
    ASSERT(0. < b && 0. < a);

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    const auto Q      = PointLonLat::make(P.lon(), P.lat(), -180.);
    const auto lambda = util::DEGREE_TO_RADIAN * Q.lon();
    const auto phi    = util::DEGREE_TO_RADIAN * Q.lat();

    const auto sp = std::sin(phi);
    const auto cp = std::sqrt(1. - sp * sp);
    const auto sl = std::abs(Q.lon()) < 180. ? std::sin(lambda) : 0.;
    const auto cl = std::abs(Q.lon()) > 90. ? std::cos(lambda) : std::sqrt(1. - sl * sl);

    const auto N_phi = a * a / std::sqrt(a * a * cp * cp + b * b * sp * sp);

    return {(N_phi + height) * cp * cl, (N_phi + height) * cp * sl, (N_phi * (b * b) / (a * a) + height) * sp};
}


}  // namespace eckit::geo::figure
