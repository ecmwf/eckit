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


#include "eckit/geo/projection/LambertConformalConic.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


LambertConformalConic::LambertConformalConic(const Spec& spec) :
    LambertConformalConic({spec.get_double("lon_0"), spec.get_double("lat_0")},
                          {spec.get_double("first_lon"), spec.get_double("first_lat")}, spec.get_double("lat_1"),
                          spec.get_double("lat_2")) {}


LambertConformalConic::LambertConformalConic(PointLonLat centre, PointLonLat first, double lat_1, double lat_2) :
    centre_(PointLonLat::make(centre.lon, centre.lat)),
    centre_r_(PointLonLatR::make_from_lonlat(centre.lon, centre.lat)),
    first_(PointLonLat::make(first.lon, first.lat)),
    first_r_(PointLonLatR::make_from_lonlat(first.lon, first.lat)),
    lat_1_(lat_1),
    lat_1_r_(lat_1 * util::DEGREE_TO_RADIAN),
    lat_2_(lat_2),
    lat_2_r_(lat_2 * util::DEGREE_TO_RADIAN) {
    ASSERT(!types::is_approximately_equal(figure().R(), 0.));

    if (types::is_approximately_equal(lat_1, -lat_2)) {
        throw ProjectionProblem(
            "LambertConformalConic: cannot have equal latitudes for standard parallels on opposite sides of equator",
            Here());
    }

    n_ = types::is_approximately_equal(lat_1, lat_2)
             ? std::sin(lat_1_r_)
             : std::log(std::cos(lat_1_r_) / std::cos(lat_2_r_))
                   / std::log(std::tan(M_PI_4 + lat_2_r_ / 2.) / std::tan(M_PI_4 + lat_1_r_ / 2.));

    if (types::is_approximately_equal(n_, 0.)) {
        throw ProjectionProblem("LambertConformalConic: cannot corretly calculate n_", Here());
    }

    f_         = (std::cos(lat_1_r_) * std::pow(std::tan(M_PI_4 + lat_1_r_ / 2.), n_)) / n_;
    rho0_bare_ = f_ * std::pow(std::tan(M_PI_4 + centre_r_.latr / 2.), -n_);
}


Point2 LambertConformalConic::fwd(const PointLonLat& p) const {
    auto q = PointLonLatR::make_from_lonlat(p.lon, p.lat);

    auto rho  = figure().R() * f_ * std::pow(std::tan(M_PI_4 + q.latr / 2.), -n_);
    auto rho0 = figure().R() * rho0_bare_;  // scaled
    auto dlam = q.lonr - centre_r_.lonr;

    return {rho * std::sin(n_ * dlam), rho0 - rho * std::cos(n_ * dlam)};
}


PointLonLat LambertConformalConic::inv(const Point2& p) const {
    auto x = p.X / figure().R();
    auto y = rho0_bare_ - p.Y / figure().R();

    if (auto rho = std::hypot(x, y); !types::is_approximately_equal(rho, 0.)) {
        if (n_ < 0.) {
            rho = -rho;
            x   = -x;
            y   = -y;
        }
        auto lonr = std::atan2(x, y) / n_ + centre_r_.lonr;
        auto latr = 2. * std::atan(std::pow(f_ / rho, 1. / n_)) - M_PI_2;

        return PointLonLat::make_from_lonlatr(lonr, latr);
    }

    return PointLonLat::make(0., n_ > 0 ? PointLonLat::RIGHT_ANGLE : -PointLonLat::RIGHT_ANGLE);
}


void LambertConformalConic::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "lcc");
    custom.set("lon_0", centre_.lon);
    custom.set("lat_0", centre_.lat);
    custom.set("first_lon", first_.lon);
    custom.set("first_lat", first_.lat);
    custom.set("lat_1", lat_1_);
    custom.set("lat_2", lat_1_);
}


}  // namespace eckit::geo::projection
