// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/LambertAzimuthalEqualArea.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<LambertAzimuthalEqualArea> PROJECTION_1("laea");
static const ProjectionRegisterType<LambertAzimuthalEqualArea> PROJECTION_2("lambert_azimuthal_equal_area");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(double lat_0, double lon_0, Figure* fig) :
    Projection(fig),
    lat_0_(lat_0),
    lon_0_(lon_0),
    a_(figure().a()),
    authalic_(figure().authalic()),
    aspect_(types::is_approximately_equal(lat_0, PointLonLat::RIGHT_ANGLE)    ? Aspect::NORTH_POLE
            : types::is_approximately_equal(lat_0, -PointLonLat::RIGHT_ANGLE) ? Aspect::SOUTH_POLE
                                                                              : Aspect::OBLIQUE) {
    const auto phi_0 = lat_0 * util::DEGREE_TO_RADIAN;
    const auto e     = figure().eccentricity();

    rq_     = std::sqrt(authalic_.qp() / 2.);
    sinb_0_ = authalic_.q(std::sin(phi_0)) / authalic_.qp();
    cosb_0_ = std::sqrt(1. - sinb_0_ * sinb_0_);
    d_      = aspect_ == Aspect::OBLIQUE
                  ? std::cos(phi_0) / (std::sqrt(1. - e * e * std::sin(phi_0) * std::sin(phi_0)) * rq_ * cosb_0_)
                  : 1.;
}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec& spec) :
    LambertAzimuthalEqualArea(spec.get_double("lat_0", 0.), spec.get_double("lon_0", 0.), FigureFactory::build(spec)) {}


PointXY LambertAzimuthalEqualArea::fwd(const PointLonLat& p) const {
    const auto dlam =
        PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) * util::DEGREE_TO_RADIAN;
    const auto q = authalic_.q(std::sin(p.lat() * util::DEGREE_TO_RADIAN));

    if (aspect_ != Aspect::OBLIQUE) {
        // Snyder (24-23 to 24-25)
        const auto sign = aspect_ == Aspect::NORTH_POLE ? 1. : -1.;
        const auto rho  = a_ * std::sqrt(std::max(0., authalic_.qp() - sign * q));
        return {rho * std::sin(dlam), -sign * rho * std::cos(dlam)};
    }

    // Snyder (24-17 to 24-19)
    const auto sinb = q / authalic_.qp();
    const auto cosb = std::sqrt(std::max(0., 1. - sinb * sinb));
    const auto b    = rq_ * std::sqrt(2. / (1. + sinb_0_ * sinb + cosb_0_ * cosb * std::cos(dlam)));

    return {a_ * b * d_ * cosb * std::sin(dlam), a_ * b / d_ * (cosb_0_ * sinb - sinb_0_ * cosb * std::cos(dlam))};
}


PointLonLat LambertAzimuthalEqualArea::inv(const PointXY& p) const {
    if (aspect_ != Aspect::OBLIQUE) {
        // Snyder (24-26, 24-27)
        const auto sign = aspect_ == Aspect::NORTH_POLE ? 1. : -1.;
        const auto rho  = std::hypot(p.X(), p.Y()) / a_;
        const auto beta = sign * std::asin(std::min(1., 1. - rho * rho / authalic_.qp()));

        return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + std::atan2(p.X(), -sign * p.Y()),
                                              authalic_.phi(authalic_.qp() * std::sin(beta)),
                                              lon_0_ - PointLonLat::FLAT_ANGLE);
    }

    // Snyder (24-28 to 24-30)
    const auto x   = p.X() / (a_ * d_);
    const auto y   = p.Y() * d_ / a_;
    const auto rho = std::hypot(x, y);

    if (types::is_approximately_equal(rho, 0.)) {
        return PointLonLat::make(lon_0_, lat_0_);
    }

    const auto ce   = 2. * std::asin(std::min(1., rho / (2. * rq_)));
    const auto beta = std::asin(std::cos(ce) * sinb_0_ + y * std::sin(ce) * cosb_0_ / rho);

    return PointLonLat::make_from_lonlatr(
        lon_0_ * util::DEGREE_TO_RADIAN +
            std::atan2(x * std::sin(ce), rho * cosb_0_ * std::cos(ce) - y * sinb_0_ * std::sin(ce)),
        authalic_.phi(authalic_.qp() * std::sin(beta)), lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& LambertAzimuthalEqualArea::type() const {
    static const std::string type{"laea"};
    return type;
}


void LambertAzimuthalEqualArea::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());

    if (!types::is_approximately_equal(lat_0_, 0.)) {
        custom.set("lat_0", lat_0_);
    }

    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
}


}  // namespace eckit::geo::projection
