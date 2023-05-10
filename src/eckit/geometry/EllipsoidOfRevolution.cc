/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/EllipsoidOfRevolution.h"

#include <cmath>
#include <ios>
// #include <limits>  // for std::numeric_limits
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

namespace {

static double normalise_longitude(double a, const double& minimum) {
    while (a < minimum) {
        a += 360;
    }
    while (a >= minimum + 360) {
        a -= 360;
    }
    return a;
}

static const double degrees_to_radians = M_PI / 180.;

static std::streamsize max_digits10 = 15 + 3;

// C++-11: std::numeric_limits<double>::max_digits10;

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Point3 EllipsoidOfRevolution::convertSphericalToCartesian(double a,
                                                          double b,
                                                          const PointLonLat& A,
                                                          double height) {
    ASSERT(a > 0.);
    ASSERT(b > 0.);

    if (!(-90. <= A.lat && A.lat <= 90.)) {
        std::ostringstream oss;
        oss.precision(max_digits10);
        oss << "Invalid latitude " << A.lat;
        throw BadValue(oss.str(), Here());
    }

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    const double lambda_deg = normalise_longitude(A.lon, -180.);
    const double lambda     = degrees_to_radians * lambda_deg;
    const double phi        = degrees_to_radians * A.lat;

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    const double N_phi = a * a / std::sqrt(a * a * cos_phi * cos_phi + b * b * sin_phi * sin_phi);

    return {(N_phi + height) * cos_phi * cos_lambda,
            (N_phi + height) * cos_phi * sin_lambda,
            (N_phi * (b * b) / (a * a) + height) * sin_phi};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry
