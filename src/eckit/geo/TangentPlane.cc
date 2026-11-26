// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/TangentPlane.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo {


namespace {


constexpr double EPS = 1e-12;


const PointXYZ Y_AXIS{0., 1., 0.};


PointXYZ normalise(const PointXYZ& p) {
    const auto n = std::sqrt(dot(p, p));
    ASSERT(n > 0.);
    return p * (1. / n);
}


}  // namespace


TangentPlane::TangentPlane(const PointXYZ& origin) : origin_(origin) {
    const auto up = normalise(origin);
    east_         = std::hypot(up.X(), up.Y()) > EPS ? normalise({-up.Y(), up.X(), 0.}) : Y_AXIS;
    north_        = cross(up, east_);
}


TangentPlane::TangentPlane(const PointXYZ& origin, const PointLonLat& geodetic) : origin_(origin) {
    const auto phi    = geodetic.lat() * util::DEGREE_TO_RADIAN;
    const auto lambda = geodetic.lon() * util::DEGREE_TO_RADIAN;

    const auto sp = std::sin(phi);
    const auto cp = std::cos(phi);
    const auto sl = std::sin(lambda);
    const auto cl = std::cos(lambda);

    if (std::abs(cp) < EPS) {
        east_  = Y_AXIS;
        north_ = cross(PointXYZ{0., 0., sp > 0. ? 1. : -1.}, east_);
        return;
    }

    east_  = {-sl, cl, 0.};
    north_ = {-sp * cl, -sp * sl, cp};
}


PointXY TangentPlane::operator()(const PointXYZ& p) const {
    const auto d = p - origin_;
    return {dot(d, east_), dot(d, north_)};
}


}  // namespace eckit::geo
