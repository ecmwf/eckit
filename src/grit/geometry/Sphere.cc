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


double Sphere::centralAngle(const Point2& Alonlat, const Point2& Blonlat) {
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

    assert(-90. <= Alonlat[1] && Alonlat[1] <= 90. && "Invalid latitude");
    assert(-90. <= Blonlat[1] && Blonlat[1] <= 90. && "Invalid latitude");

    const double phi1   = util::degrees_to_radians * Alonlat[1];
    const double phi2   = util::degrees_to_radians * Blonlat[1];
    const double lambda = util::degrees_to_radians * (Blonlat[0] - Alonlat[0]);

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


double Sphere::distance(const double& radius, const Point2& Alonlat, const Point2& Blonlat) {
    return radius * centralAngle(Alonlat, Blonlat);
}


double Sphere::distance(const double& radius, const Point3& A, const Point3& B) {
    return radius * centralAngle(radius, A, B);
}


double Sphere::area(const double& radius) {
    assert(radius > 0.);
    return 4. * M_PI * radius * radius;
}


double Sphere::area(const double& radius, const Point2& WestNorth, const Point2& EastSouth) {
    assert(radius > 0.);

    // Set longitude fraction
    double W = WestNorth[0];
    double E = util::normalise_longitude_to_minimum(EastSouth[0], W);
    double longitude_range(
        util::approximately_equal(W, E) && !util::approximately_equal(EastSouth[0], WestNorth[0]) ? 360. : E - W);
    assert(longitude_range <= 360.);

    double longitude_fraction = longitude_range / 360.;

    // Set latitude fraction
    double N = WestNorth[1];
    double S = EastSouth[1];
    assert(-90. <= N && N <= 90.);
    assert(-90. <= S && S <= 90.);
    assert(N >= S);

    double latitude_fraction = 0.5 * (std::sin(util::degrees_to_radians * N) - std::sin(util::degrees_to_radians * S));

    // Calculate area
    return area(radius) * latitude_fraction * longitude_fraction;
}


double Sphere::greatCircleLatitudeGivenLongitude(const Point2& Alonlat, const Point2& Blonlat, const double& Clon) {
    GreatCircle gc(Alonlat, Blonlat);
    auto lat = gc.latitude(Clon);
    return lat.size() == 1 ? lat[0] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::greatCircleLongitudeGivenLatitude(const Point2& Alonlat, const Point2& Blonlat, const double& Clat,
                                               double& Clon1, double& Clon2) {
    GreatCircle gc(Alonlat, Blonlat);
    auto lon = gc.longitude(Clat);

    Clon1 = lon.size() > 0 ? lon[0] : std::numeric_limits<double>::signaling_NaN();
    Clon2 = lon.size() > 1 ? lon[1] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::convertSphericalToCartesian(const double& radius, const Point2& Alonlat, Point3& B, double height) {
    assert(radius > 0.);
    assert(-90. <= Alonlat[1] && Alonlat[1] <= 90. && "Invalid latitude");

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

    const double lambda_deg = util::normalise_longitude_to_minimum(Alonlat[0], -180.);
    const double lambda     = util::degrees_to_radians * lambda_deg;
    const double phi        = util::degrees_to_radians * Alonlat[1];

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    B[0] = (radius + height) * cos_phi * cos_lambda;
    B[1] = (radius + height) * cos_phi * sin_lambda;
    B[2] = (radius + height) * sin_phi;
}


void Sphere::convertCartesianToSpherical(const double& radius, const Point3& A, Point2& Blonlat) {
    assert(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const double x = A[0];
    const double y = util::approximately_equal(A[1], 0.) ? 0. : A[1];
    const double z = std::min(radius, std::max(-radius, A[2])) / radius;

    Blonlat[0] = util::radians_to_degrees * std::atan2(y, x);
    Blonlat[1] = util::radians_to_degrees * std::asin(z);
}


}  // namespace grit::geometry
