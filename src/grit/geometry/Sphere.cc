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


#include "grit/geometry/Sphere.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

#include "grit/geometry/GreatCircle.h"
#include "grit/util.h"


namespace grit::geometry {


inline double squared(double x) {
    return x * x;
}


double Sphere::centralAngle(const PointLatLon& A, const PointLatLon& B) {
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

    const double phi1   = util::degrees_to_radians * A.lat;
    const double phi2   = util::degrees_to_radians * B.lat;
    const double lambda = util::degrees_to_radians * (B.lon - A.lon);

    const double cos_phi1   = std::cos(phi1);
    const double sin_phi1   = std::sin(phi1);
    const double cos_phi2   = std::cos(phi2);
    const double sin_phi2   = std::sin(phi2);
    const double cos_lambda = std::cos(lambda);
    const double sin_lambda = std::sin(lambda);

    const double angle = atan2(
        std::sqrt(squared(cos_phi2 * sin_lambda) + squared(cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_lambda)),
        sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_lambda);

    if (util::approximately_equal(angle, 0.)) {
        return 0.;
    }

    assert(angle > 0.);
    return angle;
}


double Sphere::centralAngle(const double& radius, const Point3& A, const Point3& B) {
    assert(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const double d2 = Point3::distance2(A, B);
    if (util::approximately_equal(d2, 0.)) {
        return 0.;
    }

    const double chord = std::sqrt(d2) / radius;
    const double angle = std::asin(chord * 0.5) * 2.;

    return angle;
}


double Sphere::distance(const double& radius, const PointLatLon& A, const PointLatLon& B) {
    return radius * centralAngle(A, B);
}


double Sphere::distance(const double& radius, const Point3& A, const Point3& B) {
    return radius * centralAngle(radius, A, B);
}


double Sphere::area(const double& radius) {
    assert(radius > 0.);
    return 4. * M_PI * radius * radius;
}


double Sphere::greatCircleLatitudeGivenLongitude(const PointLatLon& A, const PointLatLon& B, const double& lon) {
    GreatCircle gc(A, B);
    auto lats = gc.latitude(lon);
    return lats.size() == 1 ? lats[0] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::greatCircleLongitudeGivenLatitude(const PointLatLon& A, const PointLatLon& B, const double& lat,
                                               double& lon1, double& lon2) {
    GreatCircle gc(A, B);
    auto lons = gc.longitude(lat);

    lon1 = lons.size() > 0 ? lons[0] : std::numeric_limits<double>::signaling_NaN();
    lon2 = lons.size() > 1 ? lons[1] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::convertSphericalToCartesian(const double& radius, const PointLatLon& A, Point3& B, double height) {
    assert(radius > 0.);

    /*
     * See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
     * numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line).
     *
     * cos α = sqrt( 1 - sin^2 α) is better conditioned than explicit cos α, and
     * coupled with λ in [-180°, 180°[ the accuracy of the trigonometric
     * functions is the same (before converting/multiplying its angle argument
     * to radian) and explicitly chosing -180° over 180° for longitude.
     *
     * These three conditionings combined project very accurately to the sphere
     * poles and quadrants.
     */

    const double lambda_deg = util::normalise_longitude_to_minimum(A.lon, -180.);
    const double lambda     = util::degrees_to_radians * lambda_deg;
    const double phi        = util::degrees_to_radians * A.lat;

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    B.x = (radius + height) * cos_phi * cos_lambda;
    B.y = (radius + height) * cos_phi * sin_lambda;
    B.z = (radius + height) * sin_phi;
}


void Sphere::convertCartesianToSpherical(const double& radius, const Point3& A, PointLatLon& B) {
    assert(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const double x = A.x;
    const double y = util::approximately_equal(A.y, 0.) ? 0. : A.y;
    const double z = std::min(radius, std::max(-radius, A.z)) / radius;

    B.lon = util::radians_to_degrees * std::atan2(y, x);
    B.lat = util::radians_to_degrees * std::asin(z);
}


}  // namespace grit::geometry
