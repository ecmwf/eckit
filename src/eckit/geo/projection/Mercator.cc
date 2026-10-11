// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Mercator.h"

#include <cmath>
#include <limits>

#include "eckit/geo/Figure.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<Mercator> PROJECTION_1("merc");
static const ProjectionRegisterType<Mercator> PROJECTION_2("mercator");


Mercator::Mercator(double lon_0, double lat_ts, Figure* fig) :
    Projection(fig),
    lon_0_(lon_0),
    lat_ts_(lat_ts),
    conformal_(figure().eccentricity()),
    k_(figure().a() * conformal_.m(lat_ts * util::DEGREE_TO_RADIAN)) {}


Mercator::Mercator(const Spec& spec) :
    Mercator(spec.get_double("lon_0", 0.), spec.get_double("lat_ts", 0.), FigureFactory::build(spec)) {}


PointXY Mercator::fwd(const PointLonLat& p) const {
    const auto x = k_ * PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) *
                   util::DEGREE_TO_RADIAN;

    const auto y = types::is_approximately_equal(p.lat(), PointLonLat::RIGHT_ANGLE)
                       ? std::numeric_limits<double>::infinity()
                   : types::is_approximately_equal(p.lat(), -PointLonLat::RIGHT_ANGLE)
                       ? -std::numeric_limits<double>::infinity()
                       : -k_ * std::log(conformal_.t(p.lat() * util::DEGREE_TO_RADIAN));

    return {x, y};
}


PointLonLat Mercator::inv(const PointXY& q) const {
    return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + q.X() / k_,
                                          conformal_.phi(std::exp(-q.Y() / k_)), lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& Mercator::type() const {
    static const std::string type{"merc"};
    return type;
}


void Mercator::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());

    if (!types::is_approximately_equal(lat_ts_, 0.)) {
        custom.set("lat_ts", lat_ts_);
    }

    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
}


}  // namespace eckit::geo::projection
