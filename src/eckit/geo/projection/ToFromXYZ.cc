// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/ToFromXYZ.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


ToFromXYZ::ToFromXYZ(Figure* figure_ptr, const Point& source) : Projection(figure_ptr, source, PointXYZ{}) {}


PointXYZ ToFromXYZ::to_xyz(double a, double b, const PointLonLat& P, double height) {
    ASSERT(0. < a && 0. < b);

    /*
     * See https://en.wikipedia.org/wiki/Reference_ellipsoid#Coordinates
     * numerical conditioning for both ϕ (poles) and λ (Greenwich/Date Line).
     *
     * cos φ = sqrt( 1 - sin^2 φ) is better conditioned than explicit cos φ, and
     * coupled with λ in [-180°, 180°[ the accuracy of the trigonometric
     * functions is the same (before converting/multiplying its angle argument
     * to radian) and explicitly chosing -180° over 180° for longitude.
     *
     * These conditionings combined project accurately to sphere poles and quadrants.
     */

    const auto Q      = PointLonLat::make(P.lon(), P.lat(), -180.);
    const auto lambda = util::DEGREE_TO_RADIAN * Q.lon();
    const auto phi    = util::DEGREE_TO_RADIAN * Q.lat();

    const auto sp = std::sin(phi);
    const auto cp = std::sqrt(1. - sp * sp);
    const auto sl = std::abs(Q.lon()) < 180. ? std::sin(lambda) : 0.;
    const auto cl = std::abs(Q.lon()) > 90. ? std::cos(lambda) : std::sqrt(1. - sl * sl);

    // on a sphere the radius of curvature N(φ) is the radius (and exactly so)
    if (types::is_approximately_equal(a, b)) {
        return {(a + height) * cp * cl, (a + height) * cp * sl, (a + height) * sp};
    }

    const auto N_phi = a * a / std::sqrt(a * a * cp * cp + b * b * sp * sp);

    return {(N_phi + height) * cp * cl, (N_phi + height) * cp * sl, (N_phi * (b * b) / (a * a) + height) * sp};
}


PointLonLat ToFromXYZ::to_lonlat(const PointXYZ& A) {
    // numerical conditioning for y (Greenwich/Date Line); atan2 for latitude is well conditioned at the poles, and
    // doesn't depend on the distance from the centre
    const auto x = A.X();
    const auto y = types::is_approximately_equal(A.Y(), 0.) ? 0. : A.Y();

    return {util::RADIAN_TO_DEGREE * std::atan2(y, x),
            util::RADIAN_TO_DEGREE * std::atan2(A.Z(), std::hypot(x, A.Y()))};
}


PointLonLatH ToFromXYZ::to_lonlath(double R, const PointXYZ& A) {
    ASSERT(0. < R);

    const auto r = std::sqrt(dot(A, A));
    if (!(r > 0.)) {
        throw exception::ProjectionError("ToFromXYZ: point at the centre of the figure", Here());
    }

    return {to_lonlat(A), r - R};
}


std::vector<std::vector<double>> ToFromXYZ::to_xyz(const std::vector<double>& lon, const std::vector<double>& lat,
                                                   const std::vector<double>& height,
                                                   double height_scale_factor) const {
    const auto a = figure().a();
    const auto b = figure().b();

    std::vector<std::vector<double>> xyz(3, std::vector<double>(lon.size()));
    for (size_t i = 0; i < lon.size(); ++i) {
        const auto q = to_xyz(a, b, PointLonLat{lon[i], lat[i]}, height.empty() ? 0. : height_scale_factor * height[i]);
        xyz[0][i]    = q.X();
        xyz[1][i]    = q.Y();
        xyz[2][i]    = q.Z();
    }
    return xyz;
}


}  // namespace eckit::geo::projection
