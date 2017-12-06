/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/Sphere.h"

#include <algorithm>
#include <cmath>
#include <ios>
//#include <limits> // for std::numeric_limits
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/types/FloatCompare.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

static inline double between_m180_and_p180(double a)
{
    while (a > 180) {
        a -= 360;
    }
    while (a < -180 ) {
        a += 360;
    }
    return a;
}

static const double radians_to_degrees = 180. * M_1_PI;

static const double degrees_to_radians = M_PI / 180.;

static std::streamsize max_digits10 = 15 + 3;  // C++-11: std::numeric_limits<double>::max_digits10;

//------------------------------------------------------------------------------------------------------

double Sphere::centralAngle(const Point2& Alatlon, const Point2& Blatlon)
{
    using namespace std;

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

    if (!(-90. <= Alatlon[0] && Alatlon[0] <= 90.)) {
        std::ostringstream oss;
        oss.precision(max_digits10);
        oss << "Invalid latitude " << Alatlon[0];
        throw eckit::BadValue(oss.str(), Here());
    }

    if (!(-90. <= Blatlon[0] && Blatlon[0] <= 90.)) {
        std::ostringstream oss;
        oss.precision(max_digits10);
        oss << "Invalid latitude " << Blatlon[0];
        throw eckit::BadValue(oss.str(), Here());
    }

    const double phi1   = degrees_to_radians * Alatlon[0];
    const double phi2   = degrees_to_radians * Blatlon[0];
    const double lambda = degrees_to_radians * (Blatlon[1] - Alatlon[1]);

    const double cos_phi1 = cos(phi1);
    const double sin_phi1 = sin(phi1);
    const double cos_phi2 = cos(phi2);
    const double sin_phi2 = sin(phi2);
    const double cos_lambda = cos(lambda);
    const double sin_lambda = sin(lambda);

    const double angle = atan2(
                             sqrt(pow(cos_phi2 * sin_lambda, 2) +
                                  pow(cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_lambda, 2)),
                             sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_lambda );

    if (eckit::types::is_approximately_equal(angle, 0.)) {
        return 0.;
    }

    ASSERT(angle > 0.);
    return angle;
}

double Sphere::centralAngle(const double& radius, const Point3& A, const Point3& B)
{
    ASSERT(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const double d2 = Point3::distance2(A, B);
    if (eckit::types::is_approximately_equal(d2, 0.)) {
        return 0.;
    }

    const double chord = std::sqrt(d2) / radius;
    const double angle = std::asin(chord * 0.5) * 2.;

    return angle;
}

double Sphere::distanceInMeters(const double& radius, const Point2& Alatlon, const Point2& Blatlon)
{
    return radius * centralAngle(Alatlon, Blatlon);
}

double Sphere::distanceInMeters(const double& radius, const Point3& A, const Point3& B)
{
    return radius * centralAngle(radius, A, B);
}

void Sphere::greatCircleLatitudeGivenLongitude(const Point2& Alatlon, const Point2& Blatlon, Point2& Clatlon)
{
    using namespace std;

    //  Intermediate great circle points (not applicable for meridians), see
    //   http://www.edwilliams.org/avform.htm#Int
    ASSERT(!eckit::types::is_approximately_equal(Alatlon[1], Blatlon[1]));

    // NOTE: uses C longitude to set C latitude
    const double phi1     = degrees_to_radians * Alatlon[0];
    const double phi2     = degrees_to_radians * Blatlon[0];
    const double lambda1p = degrees_to_radians * (Clatlon[1] - Alatlon[1]);
    const double lambda2p = degrees_to_radians * (Clatlon[1] - Blatlon[1]);
    const double lambda   = degrees_to_radians * (Blatlon[1] - Alatlon[1]);

    Clatlon[0] = radians_to_degrees * atan(
                  (tan(phi2) * sin(lambda1p) - tan(phi1) * sin(lambda2p)) /
                  (sin(lambda)) );
}

void Sphere::convertSphericalToCartesian(const double& radius, const Point2& Alatlon, Point3& B, double height)
{
    ASSERT(radius > 0.);

    if (!(-90. <= Alatlon[0] && Alatlon[0] <= 90.)) {
        std::ostringstream oss;
        oss.precision(max_digits10);
        oss << "Invalid latitude " << Alatlon[0];
        throw eckit::BadValue(oss.str(), Here());
    }

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)
    const double& a = radius;
    const double& b = radius;

    const double lambda_deg = between_m180_and_p180(Alatlon[1]);
    const double lambda = degrees_to_radians * lambda_deg;
    const double phi    = degrees_to_radians * Alatlon[0];

    const double sin_phi = std::sin(phi);
    const double cos_phi = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) >  90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    if (eckit::types::is_approximately_equal(a,b)) { // no eccentricity case
        B[0] = (a + height) * cos_phi * cos_lambda;
        B[1] = (a + height) * cos_phi * sin_lambda;
        B[2] = (a + height) * sin_phi;
    }

    const double N_phi = a * a / std::sqrt(a * a * cos_phi * cos_phi + b * b * sin_phi * sin_phi);

    B[0] = (N_phi + height) * cos_phi * cos_lambda;
    B[1] = (N_phi + height) * cos_phi * sin_lambda;
    B[2] = (N_phi * (b * b) / (a * a) + height) * sin_phi;
}

void Sphere::convertCartesianToSpherical(const double& radius, const Point3& A, Point2& Blatlon)
{
    ASSERT(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const double x = A[0];
    const double y = eckit::types::is_approximately_equal(A[1], 0.) ? 0. : A[1];
    const double z = std::min(radius, std::max(-radius, A[2])) / radius;

    Blatlon[0] = radians_to_degrees * std::asin(z);
    Blatlon[1] = radians_to_degrees * std::atan2(y, x);
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit
