/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geo/Sphere.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/GreatCircle.h"
#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/FloatCompare.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geo {

//----------------------------------------------------------------------------------------------------------------------

using types::is_approximately_equal;

static constexpr double radians_to_degrees = 180. * M_1_PI;

static constexpr double degrees_to_radians = M_PI / 180.;

void assert_latitude(double lat) {
    if (!(-90. <= lat && lat <= 90.)) {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::max_digits10);
        oss << "Invalid latitude " << lat;
        throw BadValue(oss.str(), Here());
    }
}

inline double squared(double x) {
    return x * x;
}

//----------------------------------------------------------------------------------------------------------------------

double Sphere::centralAngle(const PointLonLat& A, const PointLonLat& B) {
    assert_latitude(A.lat);
    assert_latitude(B.lat);

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

    const double phi1   = degrees_to_radians * A.lat;
    const double phi2   = degrees_to_radians * B.lat;
    const double lambda = degrees_to_radians * (B.lon - A.lon);

    const double cos_phi1   = cos(phi1);
    const double sin_phi1   = sin(phi1);
    const double cos_phi2   = cos(phi2);
    const double sin_phi2   = sin(phi2);
    const double cos_lambda = cos(lambda);
    const double sin_lambda = sin(lambda);

    const double angle = std::atan2(
        std::sqrt(squared(cos_phi2 * sin_lambda) + squared(cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_lambda)),
        sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_lambda);

    if (is_approximately_equal(angle, 0.)) {
        return 0.;
    }

    ASSERT(angle > 0.);
    return angle;
}

double Sphere::centralAngle(double radius, const Point3& A, const Point3& B) {
    ASSERT(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const double d2 = Point3::distance2(A, B);
    if (is_approximately_equal(d2, 0.)) {
        return 0.;
    }

    const double chord = std::sqrt(d2) / radius;
    const double angle = std::asin(chord * 0.5) * 2.;

    return angle;
}

double Sphere::distance(double radius, const PointLonLat& A, const PointLonLat& B) {
    return radius * centralAngle(A, B);
}

double Sphere::distance(double radius, const Point3& A, const Point3& B) {
    return radius * centralAngle(radius, A, B);
}

double Sphere::area(double radius) {
    ASSERT(radius > 0.);
    return 4. * M_PI * radius * radius;
}

double Sphere::area(double radius, const PointLonLat& WestNorth, const PointLonLat& EastSouth) {
    ASSERT(radius > 0.);
    assert_latitude(WestNorth.lat);
    assert_latitude(EastSouth.lat);

    // Set longitude fraction
    double W = WestNorth.lon;
    double E = PointLonLat::normalise_angle_to_minimum(EastSouth.lon, W);
    double longitude_range(
        is_approximately_equal(W, E) && !is_approximately_equal(EastSouth.lon, WestNorth.lon) ? 360. : E - W);
    ASSERT(longitude_range <= 360.);

    double longitude_fraction = longitude_range / 360.;

    // Set latitude fraction
    double N = WestNorth.lat;
    double S = EastSouth.lat;
    ASSERT(S <= N);

    double latitude_fraction = 0.5 * (std::sin(degrees_to_radians * N) - std::sin(degrees_to_radians * S));

    // Calculate area
    return area(radius) * latitude_fraction * longitude_fraction;
}

double Sphere::greatCircleLatitudeGivenLongitude(const PointLonLat& A, const PointLonLat& B, double Clon) {
    GreatCircle gc(A, B);
    auto lat = gc.latitude(Clon);
    return lat.size() == 1 ? lat[0] : std::numeric_limits<double>::signaling_NaN();
}

void Sphere::greatCircleLongitudeGivenLatitude(
    const PointLonLat& A, const PointLonLat& B, double Clat, double& Clon1, double& Clon2) {
    GreatCircle gc(A, B);
    auto lon = gc.longitude(Clat);

    Clon1 = lon.size() > 0 ? lon[0] : std::numeric_limits<double>::signaling_NaN();
    Clon2 = lon.size() > 1 ? lon[1] : std::numeric_limits<double>::signaling_NaN();
}

Point3 Sphere::convertSphericalToCartesian(double radius, const PointLonLat& A, double height, bool normalise_angle) {
    ASSERT(radius > 0.);

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

    if (!normalise_angle) {
        assert_latitude(A.lat);
    }

    const auto P      = PointLonLat::make(A.lon, A.lat, -180.);
    const auto lambda = degrees_to_radians * P.lon;
    const auto phi    = degrees_to_radians * P.lat;

    const auto sin_phi    = std::sin(phi);
    const auto cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const auto sin_lambda = std::abs(P.lon) < 180. ? std::sin(lambda) : 0.;
    const auto cos_lambda = std::abs(P.lon) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    return {(radius + height) * cos_phi * cos_lambda,
            (radius + height) * cos_phi * sin_lambda,
            (radius + height) * sin_phi};
}

PointLonLat Sphere::convertCartesianToSpherical(double radius, const Point3& A) {
    ASSERT(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const double x = A[0];
    const double y = is_approximately_equal(A[1], 0.) ? 0. : A[1];
    const double z = std::min(radius, std::max(-radius, A[2])) / radius;

    return {radians_to_degrees * std::atan2(y, x), radians_to_degrees * std::asin(z)};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo
