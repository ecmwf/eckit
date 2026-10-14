// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/PolarStereographic.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<PolarStereographic> PROJECTION_1("stere");
static const ProjectionRegisterType<PolarStereographic> PROJECTION_2("polar_stereographic");


PolarStereographic::PolarStereographic(double lat_0, double lat_ts, double lon_0, Figure* fig) :
    Projection(fig),
    lat_0_(lat_0),
    lat_ts_(lat_ts),
    lon_0_(lon_0),
    sign_(lat_0 < 0. ? -1. : 1.),
    conformal_(figure().conformal()) {
    if (!types::is_approximately_equal(lat_0, sign_ * PointLonLat::RIGHT_ANGLE)) {
        throw exception::ProjectionError("PolarStereographic: lat_0 = 90 or -90 expected (polar)", Here());
    }

    const auto e     = figure().eccentricity();
    const auto phi_c = sign_ * lat_ts * util::DEGREE_TO_RADIAN;

    k_ = types::is_approximately_equal(lat_ts, lat_0)
             ? 2. * figure().a() / std::sqrt(std::pow(1. + e, 1. + e) * std::pow(1. - e, 1. - e))
             : figure().a() * conformal_.m(phi_c) / conformal_.t(phi_c);
}


PolarStereographic::PolarStereographic(const Spec& spec) :
    PolarStereographic(spec.get_double("lat_0"), spec.get_double("lat_ts", spec.get_double("lat_0")),
                       spec.get_double("lon_0", 0.), FigureFactory::build(spec)) {}


PointXY PolarStereographic::fwd(const PointLonLat& p) const {
    const auto rho = k_ * conformal_.t(sign_ * p.lat() * util::DEGREE_TO_RADIAN);
    const auto dlam =
        PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) * util::DEGREE_TO_RADIAN;

    return {rho * std::sin(dlam), -sign_ * rho * std::cos(dlam)};
}


PointLonLat PolarStereographic::inv(const PointXY& q) const {
    const auto rho = std::hypot(q.X(), q.Y());

    return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + std::atan2(q.X(), -sign_ * q.Y()),
                                          sign_ * conformal_.phi(rho / k_), lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& PolarStereographic::type() const {
    static const std::string type{"stere"};
    return type;
}


void PolarStereographic::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());
    custom.set("lat_0", lat_0_);

    if (!types::is_approximately_equal(lat_ts_, lat_0_)) {
        custom.set("lat_ts", lat_ts_);
    }

    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
}


}  // namespace eckit::geo::projection
