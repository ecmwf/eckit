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


#include "eckit/geo/projection/figure/LambertConformalConic.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/geometry/Earth.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection::figure {


LambertConformalConic::LambertConformalConic(const Spec& spec) :
    LambertConformalConic({spec.get_double("lon_0"), spec.get_double("lat_0")}, spec.get_double("lat_1"),
                          spec.get_double("lat_2")) {}


LambertConformalConic::LambertConformalConic(PointLonLat centre, double lat_1, double lat_2) :
    centre_(centre),
    centre_r_(centre),
    lat_1_(lat_1),
    lat_2_(lat_2),
    lat_1_r_(lat_1 * util::DEGREE_TO_RADIAN),
    lat_2_r_(lat_2 * util::DEGREE_TO_RADIAN) {
    ASSERT(!eckit::types::is_approximately_equal(figure().R(), 0.));

    if (eckit::types::is_approximately_equal(lat_1, -lat_2)) {
        throw ProjectionProblem(
            "LambertConformalConic: cannot have equal latitudes for standard parallels on opposite sides of equator",
            Here());
    }

    double latFirstInRadians = 0.;
    double lonFirstInRadians = 0.;
    double LaDInRadians      = 0.;
    double LoVInRadians      = 0.;

    auto n = eckit::types::is_approximately_equal(lat_1, lat_2)
                 ? sin(lat_1_r_)
                 : log(cos(lat_1_r_) / cos(lat_2_r_)) / log(tan(M_PI_4 + lat_2_r_ / 2.) / tan(M_PI_4 + lat_1_r_ / 2.));

    auto f         = (cos(lat_1_r_) * pow(tan(M_PI_4 + lat_1_r_ / 2.), n)) / n;
    auto rho0_bare = f * pow(tan(M_PI_4 + LaDInRadians / 2.), -n);
    auto rho       = figure().R() * f * pow(tan(M_PI_4 + latFirstInRadians / 2.), -n);
    auto rho0      = figure().R() * rho0_bare;  // scaled

    auto dlam = lonFirstInRadians - LoVInRadians;

    auto x0 = rho * sin(n * dlam);
    auto y0 = rho0 - rho * cos(n * dlam);
}


Point2 LambertConformalConic::fwd(const PointLonLat&) const {
    NOTIMP;
}


PointLonLat LambertConformalConic::inv(const Point2& p) const {
    double n            = 0.;
    double f            = 0;
    double rho0_bare    = 0;
    double LoVInRadians = 0;

    ASSERT(n != 0.);

    double x = p.X / figure().R();
    double y = rho0_bare - p.Y / figure().R();

    if (double rho = std::hypot(x, y); rho != 0.) {
        if (n < 0.) {
            rho = -rho;
            x   = -x;
            y   = -y;
        }
        double lonr = std::atan2(x, y) / n + LoVInRadians;
        double latr = 2. * std::atan(pow(f / rho, 1. / n)) - M_PI_2;

        return PointLonLat::make(util::RADIAN_TO_DEGREE * lonr, util::RADIAN_TO_DEGREE * latr);
    }

    return PointLonLat::make(0., n > 0 ? PointLonLat::NORTH_POLE : PointLonLat::SOUTH_POLE);
}


Spec* LambertConformalConic::spec() const {
    NOTIMP;
}


}  // namespace eckit::geo::projection::figure
