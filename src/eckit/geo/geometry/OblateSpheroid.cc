/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/geometry/OblateSpheroid.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"


namespace eckit::geo::geometry {


double OblateSpheroid::eccentricity(double a, double b) {
    ASSERT(0. < b && b <= a);
    return std::sqrt(1. - b * b / (a * a));
}


double OblateSpheroid::flattening(double a, double b) {
    ASSERT(0. < b && b <= a);
    return (a - b) / a;
}


Point3 OblateSpheroid::convertSphericalToCartesian(double a, double b, const PointLonLat& P, double height) {
    ASSERT(0. < b && 0. < a);

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    const auto Q      = PointLonLat::make(P.lon, P.lat, -180.);
    const auto lambda = util::DEGREE_TO_RADIAN * Q.lon;
    const auto phi    = util::DEGREE_TO_RADIAN * Q.lat;

    const auto sp = std::sin(phi);
    const auto cp = std::sqrt(1. - sp * sp);
    const auto sl = std::abs(Q.lon) < 180. ? std::sin(lambda) : 0.;
    const auto cl = std::abs(Q.lon) > 90. ? std::cos(lambda) : std::sqrt(1. - sl * sl);

    const auto N_phi = a * a / std::sqrt(a * a * cp * cp + b * b * sp * sp);

    return {(N_phi + height) * cp * cl, (N_phi + height) * cp * sl, (N_phi * (b * b) / (a * a) + height) * sp};
}


}  // namespace eckit::geo::geometry
