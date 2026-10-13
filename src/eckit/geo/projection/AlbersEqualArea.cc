// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/AlbersEqualArea.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<AlbersEqualArea> PROJECTION_1("aea");
static const ProjectionRegisterType<AlbersEqualArea> PROJECTION_2("albers");


AlbersEqualArea::AlbersEqualArea(const Spec& spec) :
    AlbersEqualArea(spec.get_double("lon_0"), spec.get_double("lat_0"), spec.get_double("lat_1"),
                    spec.get_double("lat_2"), FigureFactory::build(spec)) {}


AlbersEqualArea::AlbersEqualArea(double lon_0, double lat_0, double lat_1, double lat_2, Figure* _figure) :
    Projection(_figure),
    centre_(lon_0, lat_0),
    centre_r_(PointLonLatR::make_from_lonlat(lon_0, lat_0)),
    lat_1_(lat_1),
    lat_2_(lat_2),
    a_(figure().a()),
    authalic_(figure().authalic()) {
    const auto conformal = figure().conformal();

    const auto phi_1 = lat_1 * util::DEGREE_TO_RADIAN;
    const auto phi_2 = lat_2 * util::DEGREE_TO_RADIAN;
    const auto m_1   = conformal.m(phi_1);
    const auto q_1   = authalic_.q(std::sin(phi_1));

    // a tangent cone (one standard parallel) or a secant cone
    n_ = types::is_approximately_equal(lat_1, lat_2)
             ? std::sin(phi_1)
             : (m_1 * m_1 - conformal.m(phi_2) * conformal.m(phi_2)) / (authalic_.q(std::sin(phi_2)) - q_1);

    if (types::is_approximately_equal(n_, 0.)) {
        throw exception::ProjectionError("AlbersEqualArea: cannot calculate n", Here());
    }

    C_    = m_1 * m_1 + n_ * q_1;
    rho0_ = std::sqrt(C_ - n_ * authalic_.q(std::sin(centre_r_.latr()))) / n_;
}


PointXY AlbersEqualArea::fwd(const PointLonLat& p) const {
    const auto c = C_ - n_ * authalic_.q(std::sin(p.lat() * util::DEGREE_TO_RADIAN));
    if (c < 0.) {
        throw exception::ProjectionError("AlbersEqualArea: point not projectable", Here());
    }

    const auto rho    = std::sqrt(c) / n_;
    const auto thetar = n_ *
                        PointLonLat::normalise_angle_to_minimum(p.lon() - centre_.lon(), -PointLonLat::FLAT_ANGLE) *
                        util::DEGREE_TO_RADIAN;

    return PointXY{a_ * rho * std::sin(thetar), a_ * (rho0_ - rho * std::cos(thetar))};
}


PointLonLat AlbersEqualArea::inv(const PointXY& q) const {
    auto x = q.X() / a_;
    auto y = rho0_ - q.Y() / a_;

    auto rho = std::hypot(x, y);
    if (types::is_approximately_equal(rho, 0.)) {
        return PointLonLat::make(centre_.lon(), n_ > 0. ? PointLonLat::RIGHT_ANGLE : -PointLonLat::RIGHT_ANGLE,
                                 centre_.lon() - PointLonLat::FLAT_ANGLE);
    }

    if (n_ < 0.) {
        rho = -rho;
        x   = -x;
        y   = -y;
    }

    return PointLonLat::make_from_lonlatr(centre_r_.lonr() + std::atan2(x, y) / n_,
                                          authalic_.phi((C_ - rho * rho * n_ * n_) / n_),
                                          centre_.lon() - PointLonLat::FLAT_ANGLE);
}


const std::string& AlbersEqualArea::type() const {
    static const std::string type{"aea"};
    return type;
}


void AlbersEqualArea::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());
    custom.set("lon_0", centre_.lon());
    custom.set("lat_0", centre_.lat());
    custom.set("lat_1", lat_1_);
    custom.set("lat_2", lat_2_);
}


}  // namespace eckit::geo::projection
