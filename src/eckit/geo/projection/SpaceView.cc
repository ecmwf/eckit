// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/SpaceView.h"

#include <cmath>
#include <limits>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<SpaceView> PROJECTION("space_view");


SpaceView::SpaceView(double h, double lon_0, const std::string& sweep, Figure* fig) :
    Projection(fig),
    h_(h),
    lon_0_(lon_0),
    sweep_x_(sweep == "x"),
    radius_g_(1. + h / figure().a()),
    radius_p_(figure().b() / figure().a()) {
    if (!types::is_strictly_greater(h, 0.) || (sweep != "x" && sweep != "y")) {
        throw exception::ProjectionError("SpaceView: h > 0 and sweep = x or y expected", Here());
    }
}


SpaceView::SpaceView(const Spec& spec) :
    SpaceView(spec.get_double("h"), spec.get_double("lon_0", 0.), spec.get_string("sweep", "y"),
              FigureFactory::build(spec)) {}


PointXY SpaceView::fwd(const PointLonLat& p) const {
    const auto lam =
        PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) * util::DEGREE_TO_RADIAN;
    const auto phi = std::atan(radius_p_ * radius_p_ * std::tan(p.lat() * util::DEGREE_TO_RADIAN));  // geocentric

    // satellite view vector
    const auto r  = radius_p_ / std::hypot(radius_p_ * std::cos(phi), std::sin(phi));
    const auto vx = r * std::cos(lam) * std::cos(phi);
    const auto vy = r * std::sin(lam) * std::cos(phi);
    const auto vz = r * std::sin(phi);

    if ((radius_g_ - vx) * vx - vy * vy - vz * vz / (radius_p_ * radius_p_) < 0.) {
        constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
        return {nan, nan};
    }

    const auto tmp = radius_g_ - vx;
    return sweep_x_ ? PointXY{h_ * std::atan(vy / std::hypot(vz, tmp)), h_ * std::atan(vz / tmp)}
                    : PointXY{h_ * std::atan(vy / tmp), h_ * std::atan(vz / std::hypot(vy, tmp))};
}


PointLonLat SpaceView::inv(const PointXY& q) const {
    const auto x = q.X() / h_;
    const auto y = q.Y() / h_;

    auto vx = -1.;
    auto vy = sweep_x_ ? std::tan(x) * std::hypot(1., std::tan(y)) : std::tan(x);
    auto vz = sweep_x_ ? std::tan(y) : std::tan(y) * std::hypot(1., vy);

    // intersection with the ellipsoid
    const auto a   = vx * vx + vy * vy + vz * vz / (radius_p_ * radius_p_);
    const auto b   = 2. * radius_g_ * vx;
    const auto det = b * b - 4. * a * (radius_g_ * radius_g_ - 1.);

    if (det < 0.) {
        constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
        return {nan, nan};
    }

    const auto k = (-b - std::sqrt(det)) / (2. * a);
    vx           = radius_g_ + k * vx;
    vy *= k;
    vz *= k;

    const auto lam = std::atan2(vy, vx);
    const auto phi = std::atan(vz * std::cos(lam) / vx / (radius_p_ * radius_p_));  // geodetic

    return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + lam, phi, lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& SpaceView::type() const {
    static const std::string type{"space_view"};
    return type;
}


void SpaceView::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());
    custom.set("h", h_);

    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }

    if (sweep_x_) {
        custom.set("sweep", "x");
    }
}


}  // namespace eckit::geo::projection
