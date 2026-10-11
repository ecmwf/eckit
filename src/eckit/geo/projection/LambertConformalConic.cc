// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/LambertConformalConic.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<LambertConformalConic> PROJECTION_1("lcc");
static const ProjectionRegisterType<LambertConformalConic> PROJECTION_2("lambert");
static const ProjectionRegisterType<LambertConformalConic> PROJECTION_3("lambert_lam");


LambertConformalConic::LambertConformalConic(const Spec& spec) :
    LambertConformalConic(spec.get_double("lat_1"), spec.get_double("lat_2", spec.get_double("lat_1")),
                          spec.get_double("lon_0", 0.), spec.get_double("lat_0", 0.), FigureFactory::build(spec)) {}


LambertConformalConic::LambertConformalConic(double lat_1, double lat_2, double lon_0, double lat_0, Figure* fig) :
    Projection(fig), lon_0_(lon_0), lat_0_(lat_0), lat_1_(lat_1), lat_2_(lat_2), conformal_(figure().eccentricity()) {
    if (types::is_approximately_equal(lat_1, -lat_2)) {
        throw exception::ProjectionError(
            "LambertConformalConic: cannot have standard parallels on opposite sides of equator", Here());
    }

    const auto phi_1 = lat_1 * util::DEGREE_TO_RADIAN;
    const auto phi_2 = lat_2 * util::DEGREE_TO_RADIAN;
    const auto m_1   = conformal_.m(phi_1);
    const auto t_1   = conformal_.t(phi_1);

    n_ = types::is_approximately_equal(lat_1, lat_2)
             ? std::sin(phi_1)
             : std::log(m_1 / conformal_.m(phi_2)) / std::log(t_1 / conformal_.t(phi_2));

    if (types::is_approximately_equal(n_, 0.)) {
        throw exception::ProjectionError("LambertConformalConic: cannot calculate n", Here());
    }

    aF_   = figure().a() * m_1 / (n_ * std::pow(t_1, n_));
    rho0_ = aF_ * std::pow(conformal_.t(lat_0 * util::DEGREE_TO_RADIAN), n_);
}


PointXY LambertConformalConic::fwd(const PointLonLat& p) const {
    const auto rho   = aF_ * std::pow(conformal_.t(p.lat() * util::DEGREE_TO_RADIAN), n_);
    const auto theta = n_ * PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) *
                       util::DEGREE_TO_RADIAN;

    return {rho * std::sin(theta), rho0_ - rho * std::cos(theta)};
}


PointLonLat LambertConformalConic::inv(const PointXY& q) const {
    // signs as n, Snyder (14-10)
    const auto sign = n_ < 0. ? -1. : 1.;
    const auto x    = sign * q.X();
    const auto y    = sign * (rho0_ - q.Y());
    const auto rho  = sign * std::hypot(x, y);

    if (types::is_approximately_equal(rho, 0.)) {
        return PointLonLat::make(lon_0_, sign * PointLonLat::RIGHT_ANGLE);
    }

    return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + std::atan2(x, y) / n_,
                                          conformal_.phi(std::pow(rho / aF_, 1. / n_)),
                                          lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& LambertConformalConic::type() const {
    static const std::string type{"lcc"};
    return type;
}


void LambertConformalConic::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());

    custom.set("lat_1", lat_1_);
    if (!types::is_approximately_equal(lat_2_, lat_1_)) {
        custom.set("lat_2", lat_2_);
    }
    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
    if (!types::is_approximately_equal(lat_0_, 0.)) {
        custom.set("lat_0", lat_0_);
    }
}


}  // namespace eckit::geo::projection
