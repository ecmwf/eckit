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


#include <cmath>

#include "grit/exception.h"
#include "grit/types.h"
#include "grit/util.h"


namespace grit::geometry {


void spherical_to_cartesian(Point3& B, double lon, double lat, double a, double b, double height) {
    ASSERT(a > 0.);
    ASSERT(b > 0.);
    ASSERT_MSG(-90. <= lat && lat <= 90., "Invalid latitude");

    // See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
    // numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line)

    const double lambda_deg = util::normalise_longitude_to_minimum(lon, -180.);
    const double lambda     = util::degrees_to_radians * lambda_deg;
    const double phi        = util::degrees_to_radians * lat;

    const double sin_phi    = std::sin(phi);
    const double cos_phi    = std::sqrt(1. - sin_phi * sin_phi);
    const double sin_lambda = std::abs(lambda_deg) < 180. ? std::sin(lambda) : 0.;
    const double cos_lambda = std::abs(lambda_deg) > 90. ? std::cos(lambda) : std::sqrt(1. - sin_lambda * sin_lambda);

    const double N_phi = a * a / std::sqrt(a * a * cos_phi * cos_phi + b * b * sin_phi * sin_phi);

    B.x = (N_phi + height) * cos_phi * cos_lambda;
    B.y = (N_phi + height) * cos_phi * sin_lambda;
    B.z = (N_phi * (b * b) / (a * a) + height) * sin_phi;
}


}  // namespace grit::geometry
