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

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/CoordinateHelpers.h"
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

namespace {

static const double degrees_to_radians = M_PI / 180.;

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

void EllipsoidOfRevolution::convertSphericalToCartesian(const double& a,
                                                        const double& b,
                                                        const Point2& Alonlat,
                                                        Point3& B,
                                                        double height,
                                                        bool normalise_angle) {
    ASSERT(a > 0.);
    ASSERT(b > 0.);

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    if (!normalise_angle) {
        assert_latitude_range(Alonlat[1]);
    }

    const Point2 alonlat = canonicaliseOnSphere(Alonlat, -180.);

    const double lambda_deg = alonlat[0];
    const double lambda     = degrees_to_radians * lambda_deg;
    const double phi        = degrees_to_radians * alonlat[1];

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

}  // namespace eckit::geometry
