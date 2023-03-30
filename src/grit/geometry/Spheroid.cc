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


#include "grit/geometry/Spheroid.h"

#include <cmath>

#include "grit/exception.h"
#include "grit/util.h"


namespace grit::geometry {


Point3 Spheroid::ll_to_xyz(double a, double b, const PointLatLon& P, double height) {
    ASSERT(a > 0.);
    ASSERT(b > 0.);

    /*
     * See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
     * numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)
     *
     * cos α = sqrt( 1 - sin^2 α) is better conditioned than explicit cos α, and
     * coupled with λ in [-180°, 180°[ the accuracy of the trigonometric
     * functions is the same (before converting/multiplying its angle argument
     * to radian) and explicitly chosing -180° over 180° for longitude.
     *
     * These three conditionings combined project very accurately to the sphere
     * poles and quadrants.
     */

    const double lambda_deg = util::normalise_longitude_to_minimum(P.lon, -180.);
    const double lambda     = util::degrees_to_radians * lambda_deg;
    const double phi        = util::degrees_to_radians * P.lat;

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    if (util::approximately_equal(a, b)) {
        return {(a + height) * cos_phi * cos_lambda, (a + height) * cos_phi * sin_lambda, (a + height) * sin_phi};
    }

    const double N_phi = a * a / std::sqrt(a * a * cos_phi * cos_phi + b * b * sin_phi * sin_phi);
    return {(N_phi + height) * cos_phi * cos_lambda, (N_phi + height) * cos_phi * sin_lambda,
            (N_phi * (b * b) / (a * a) + height) * sin_phi};
}


}  // namespace grit::geometry
