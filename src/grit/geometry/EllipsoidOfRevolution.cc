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
//#include <limits>  // for std::numeric_limits
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

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

void EllipsoidOfRevolution::convertSphericalToCartesian(const double& a, const double& b, const Point2& Alonlat,
                                                        Point3& B, double height) {
    ASSERT(a > 0.);
    ASSERT(b > 0.);

    if (!(-90. <= Alonlat[1] && Alonlat[1] <= 90.)) {
        std::ostringstream oss;
        oss.precision(max_digits10);
        oss << "Invalid latitude " << Alonlat[1];
        throw BadValue(oss.str(), Here());
    }

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    const double lambda_deg = normalise_longitude(Alonlat[0], -180.);
    const double lambda     = degrees_to_radians * lambda_deg;
    const double phi        = degrees_to_radians * Alonlat[1];

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    const double N_phi = a * a / std::sqrt(a * a * cos_phi * cos_phi + b * b * sin_phi * sin_phi);

    B[0] = (N_phi + height) * cos_phi * cos_lambda;
    B[1] = (N_phi + height) * cos_phi * sin_lambda;
    B[2] = (N_phi * (b * b) / (a * a) + height) * sin_phi;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
