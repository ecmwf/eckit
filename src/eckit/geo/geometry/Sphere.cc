/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/geometry/Sphere.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/GreatCircle.h"
#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::geometry {


using types::is_approximately_equal;


double Sphere::centralAngle(const PointLonLat& P1, const PointLonLat& P2) {
    /*
     * Δσ = atan( ((cos(ϕ2) * sin(Δλ))^2 + (cos(ϕ1) * sin(ϕ2) - sin(ϕ1) * cos(ϕ2) * cos(Δλ))^2) /
     *            (sin(ϕ1) * sin(ϕ2) + cos(ϕ1) * cos(ϕ2) * cos(Δλ)) )
     *
     * @article{doi:10.1179/sre.1975.23.176.88,
     * author = {T. Vincenty},
     * title = {Direct and Inverse Solutions of Geodesics on the Ellipsoid With Application of Nested Equations},
     * journal = {Survey Review},
     * volume = {23},
     * number = {176},
     * pages = {88-93},
     * year = {1975},
     * doi = {10.1179/sre.1975.23.176.88}
     * }
     */

    const auto Q1 = PointLonLat::make(P1.lon, P1.lat, -180.);
    const auto Q2 = PointLonLat::make(P2.lon, P2.lat, -180.);

    const auto phi1   = util::DEGREE_TO_RADIAN * Q1.lat;
    const auto phi2   = util::DEGREE_TO_RADIAN * Q2.lat;
    const auto lambda = util::DEGREE_TO_RADIAN * PointLonLat::normalise_angle_to_minimum(Q1.lon - Q2.lon, -180.);

    const auto cp1 = std::cos(phi1);
    const auto sp1 = std::sin(phi1);
    const auto cp2 = std::cos(phi2);
    const auto sp2 = std::sin(phi2);
    const auto cl  = std::cos(lambda);
    const auto sl  = std::sin(lambda);

    auto squared = [](double x) { return x * x; };

    const auto angle
        = std::atan2(std::sqrt(squared(cp2 * sl) + squared(cp1 * sp2 - sp1 * cp2 * cl)), sp1 * sp2 + cp1 * cp2 * cl);

    if (is_approximately_equal(angle, 0.)) {
        return 0.;
    }

    ASSERT(angle > 0.);
    return angle;
}


double Sphere::centralAngle(double radius, const Point3& P1, const Point3& P2) {
    ASSERT(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const auto d2 = Point3::distance2(P1, P2);
    if (is_approximately_equal(d2, 0.)) {
        return 0.;
    }

    const auto chord = std::sqrt(d2) / radius;
    const auto angle = std::asin(chord * 0.5) * 2.;

    return angle;
}


double Sphere::distance(double radius, const PointLonLat& P1, const PointLonLat& P2) {
    return radius * centralAngle(P1, P2);
}


double Sphere::distance(double radius, const Point3& P1, const Point3& P2) {
    return radius * centralAngle(radius, P1, P2);
}


double Sphere::area(double radius) {
    ASSERT(radius > 0.);
    return 4. * M_PI * radius * radius;
}


double Sphere::area(double radius, const area::BoundingBox& bbox) {
    ASSERT(radius > 0.);

    // Set longitude and latitude fractions
    auto lonf = bbox.periodic() ? 1. : (bbox.east - bbox.west) / PointLonLat::FULL_ANGLE;
    ASSERT(0. <= lonf && lonf <= 1.);

    auto sn   = std::sin(util::DEGREE_TO_RADIAN * bbox.north);
    auto ss   = std::sin(util::DEGREE_TO_RADIAN * bbox.south);
    auto latf = 0.5 * (sn - ss);

    // Calculate area
    return area(radius) * latf * lonf;
}


double Sphere::greatCircleLatitudeGivenLongitude(const PointLonLat& P1, const PointLonLat& P2, double Clon) {
    GreatCircle gc(P1, P2);
    auto lat = gc.latitude(Clon);
    return lat.size() == 1 ? lat[0] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::greatCircleLongitudeGivenLatitude(const PointLonLat& P1, const PointLonLat& P2, double Clat, double& Clon1,
                                               double& Clon2) {
    GreatCircle gc(P1, P2);
    auto lon = gc.longitude(Clat);

    Clon1 = lon.size() > 0 ? lon[0] : std::numeric_limits<double>::signaling_NaN();
    Clon2 = lon.size() > 1 ? lon[1] : std::numeric_limits<double>::signaling_NaN();
}


Point3 Sphere::convertSphericalToCartesian(double radius, const PointLonLat& P, double height) {
    ASSERT(radius > 0.);

    /*
     * See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
     * numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line).
     *
     * cos φ = sqrt( 1 - sin^2 φ) is better conditioned than explicit cos φ, and
     * coupled with λ in [-180°, 180°[ the accuracy of the trigonometric
     * functions is the same (before converting/multiplying its angle argument
     * to radian) and explicitly chosing -180° over 180° for longitude.
     *
     * These conditionings combined project accurately to sphere poles and quadrants.
     */

    const auto Q      = PointLonLat::make(P.lon, P.lat, -180.);
    const auto lambda = util::DEGREE_TO_RADIAN * Q.lon;
    const auto phi    = util::DEGREE_TO_RADIAN * Q.lat;

    const auto sp = std::sin(phi);
    const auto cp = std::sqrt(1. - sp * sp);
    const auto sl = std::abs(Q.lon) < 180. ? std::sin(lambda) : 0.;
    const auto cl = std::abs(Q.lon) > 90. ? std::cos(lambda) : std::sqrt(1. - sl * sl);

    return {(radius + height) * cp * cl, (radius + height) * cp * sl, (radius + height) * sp};
}


PointLonLat Sphere::convertCartesianToSpherical(double radius, const Point3& A) {
    ASSERT(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const auto x = A[0];
    const auto y = is_approximately_equal(A[1], 0.) ? 0. : A[1];
    const auto z = std::min(radius, std::max(-radius, A[2])) / radius;

    return {util::RADIAN_TO_DEGREE * std::atan2(y, x), util::RADIAN_TO_DEGREE * std::asin(z)};
}


}  // namespace eckit::geo::geometry
