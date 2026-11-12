// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Stretch.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<Stretch> PROJECTION_1("stretch", true);
static ProjectionRegisterType<Stretch> PROJECTION_2("schmidt", true);


Stretch::Stretch(double c, bool reverse) :
    Projection(nullptr, PointLonLat{}, PointLonLat{}),
    c_(!(c > 0.) ? throw exception::ProjectionError("Stretch: factor > 0", Here())
       : reverse ? 1. / c
                 : c),
    C_((1. - c_ * c_) / (1. + c_ * c_)),
    S_(2. * c_ / (1. + c_ * c_)) {}


namespace {


double factor_from_spec(const Projection::Spec& spec) {
    if (double c = 0.; spec.get("factor", c)) {
        return c;
    }
    throw exception::SpecError("Stretch: {factor: c} (c > 0) expected", Here());
}


}  // namespace


Stretch::Stretch(const Spec& spec) : Stretch(factor_from_spec(spec), spec.get_bool("reverse", false)) {}


const std::string& Stretch::type() const {
    static const std::string type{"stretch"};
    return type;
}


Point Stretch::fwd(const Point& p) const {
    return std::holds_alternative<PointXYZ>(p) ? Point{fwd(std::get<PointXYZ>(p))}
                                               : Point{fwd(std::get<PointLonLat>(p))};
}


Point Stretch::inv(const Point& q) const {
    return std::holds_alternative<PointXYZ>(q) ? Point{inv(std::get<PointXYZ>(q))}
                                               : Point{inv(std::get<PointLonLat>(q))};
}


double Stretch::factor_from_equatorial_latitude(double lat) {
    if (!(-PointLonLat::RIGHT_ANGLE < lat && lat < PointLonLat::RIGHT_ANGLE)) {
        throw exception::ProjectionError("Stretch: equatorial latitude must be in (-90, 90)", Here());
    }

    const auto s = std::sin(util::DEGREE_TO_RADIAN * lat);
    return std::sqrt((1. - s) / (1. + s));
}


/**
 * A polar stereographic projection (of radius tan(colat / 2)), scaled by c, projected back to the sphere: as
 * sin(lat') = (C + sin(lat)) / (1 + C sin(lat)) and cos(lat') = S cos(lat) / (1 + C sin(lat)), with
 * C = (1 - c^2) / (1 + c^2) and S = 2 c / (1 + c^2), lat' = atan2(C + sin(lat), S cos(lat)) is well conditioned also
 * near the poles
 */
PointLonLat Stretch::stretch(const PointLonLat& p, double C, double S) {
    const auto lat = util::DEGREE_TO_RADIAN * p.lat();
    return PointLonLat::make(p.lon(), util::RADIAN_TO_DEGREE * std::atan2(C + std::sin(lat), S * std::cos(lat)));
}


PointXYZ Stretch::stretch(const PointXYZ& p, double C, double S) {
    const auto r = std::sqrt(dot(p, p));
    if (types::is_approximately_equal(r, 0.)) {
        return p;
    }

    // sin(lat) = z / r, and cos(lat') = S cos(lat) / (1 + C sin(lat)) scales x and y
    const auto d = 1. + C * p.Z() / r;
    const auto f = S / d;
    return {f * p.X(), f * p.Y(), (C * r + p.Z()) / d};
}


void Stretch::fill_spec(spec::Custom& spec) const {
    spec.set("type", "stretch");
    spec.set("factor", c_);
}


}  // namespace eckit::geo::projection
