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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<LambertConformalConic> PROJECTION_1("lcc");
static const ProjectionRegisterType<LambertConformalConic> PROJECTION_2("lambert");
static const ProjectionRegisterType<LambertConformalConic> PROJECTION_3("lambert_lam");


LambertConformalConic::LambertConformalConic(const Spec& spec) :
    LambertConformalConic(spec.get_double("lat_1"), spec.get_double("lon_0", 0.), spec.get_double("lat_0", 0.),
                          spec.get_double("lat_2", 0.), FigureFactory::build(spec)) {}


LambertConformalConic::LambertConformalConic(double lat_1, double lon_0, double lat_0, double lat_2, Figure* fig) :
    Projection(fig), lon_0_(lon_0), lat_0_(lat_0), lat_1_(lat_1), lat_2_(lat_2) {
    ASSERT(types::is_strictly_greater(figure().R(), 0.));

    auto lat_1r = lat_1 * util::DEGREE_TO_RADIAN;
    auto lat_2r = lat_2 * util::DEGREE_TO_RADIAN;

    if (types::is_approximately_equal(lat_1, -lat_2)) {
        throw exception::ProjectionError(
            "LambertConformalConic: cannot have standard parallels on opposite sides of equator", Here());
    }

    n_ = types::is_approximately_equal(lat_1, lat_2)
             ? std::sin(lat_1r)
             : std::log(std::cos(lat_1r) / std::cos(lat_2r)) /
                   std::log(std::tan(M_PI_4 + lat_2r / 2.) / std::tan(M_PI_4 + lat_1r / 2.));

    if (types::is_approximately_equal(n_, 0.)) {
        throw exception::ProjectionError("LambertConformalConic: cannot calculate n", Here());
    }

    f_         = (std::cos(lat_1r) * std::pow(std::tan(M_PI_4 + lat_1r / 2.), n_)) / n_;
    rho0_bare_ = f_ * std::pow(std::tan(M_PI_4 + lat_0_ * util::DEGREE_TO_RADIAN / 2.), -n_);
}


PointXY LambertConformalConic::fwd(const PointLonLat& p) const {
    auto q = PointLonLatR::make_from_lonlat(p.lon, p.lat);

    auto rho  = figure().R() * f_ * std::pow(std::tan(M_PI_4 + q.latr / 2.), -n_);
    auto rho0 = figure().R() * rho0_bare_;  // scaled
    auto dlam = q.lonr - lon_0_ * util::DEGREE_TO_RADIAN;

    return {rho * std::sin(n_ * dlam), rho0 - rho * std::cos(n_ * dlam)};
}


PointLonLat LambertConformalConic::inv(const PointXY& p) const {
    auto x = p.X / figure().R();
    auto y = rho0_bare_ - p.Y / figure().R();

    if (auto rho = std::hypot(x, y); !types::is_approximately_equal(rho, 0.)) {
        if (n_ < 0.) {
            rho = -rho;
            x   = -x;
            y   = -y;
        }
        auto lonr = std::atan2(x, y) / n_ + lon_0_ * util::DEGREE_TO_RADIAN;
        auto latr = 2. * std::atan(std::pow(f_ / rho, 1. / n_)) - M_PI_2;

        return PointLonLat::make_from_lonlatr(lonr, latr, lon_0_ - PointLonLat::FLAT_ANGLE);
    }

    return PointLonLat::make(0., n_ > 0 ? PointLonLat::RIGHT_ANGLE : -PointLonLat::RIGHT_ANGLE);
}


const std::string& LambertConformalConic::type() const {
    static const std::string type{"lcc"};
    return type;
}


void LambertConformalConic::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());

    custom.set("lat_1", lat_1_);
    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
    if (!types::is_approximately_equal(lat_0_, 0.)) {
        custom.set("lat_0", lat_0_);
    };
    if (!types::is_approximately_equal(lat_2_, 0.)) {
        custom.set("lat_2", lat_2_);
    }
}


}  // namespace eckit::geo::projection
