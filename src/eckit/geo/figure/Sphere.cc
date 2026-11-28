// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/Sphere.h"

#include <cmath>
#include <limits>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/GreatCircle.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Spec.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::figure {


Sphere::Sphere(double R) : R_(R) {
    if (!(0. < R_)) {
        throw BadValue("Sphere::R requires 0 < R", Here());
    }
}


Sphere::Sphere(const Spec& spec) : Sphere(spec.get_double("R")) {}


double Sphere::centralAngle(const PointLonLat& P1, const PointLonLat& P2) {
    /*
     * Δσ = atan( ((cos(ϕ2) * sin(Δλ))^2 + (cos(ϕ1) * sin(ϕ2) - sin(ϕ1) * cos(ϕ2) * cos(Δλ))^2) /
     *            (sin(ϕ1) * sin(ϕ2) + cos(ϕ1) * cos(ϕ2) * cos(Δλ)) )
     *
     * @article{doi:10.1179/sre.1975.23.176.88,
     * author = {T. Vincenty},
     * title = {Direct and Inverse Solutions of Geodesics on the Ellipsoid With Application of Nested Equations},
     * journal = {Survey Review},
     * volume = {23},
     * number = {176},
     * pages = {88-93},
     * year = {1975},
     * doi = {10.1179/sre.1975.23.176.88}
     * }
     */

    const auto Q1 = PointLonLat::make(P1.lon(), P1.lat(), -180.);
    const auto Q2 = PointLonLat::make(P2.lon(), P2.lat(), -180.);

    const auto phi1   = util::DEGREE_TO_RADIAN * Q1.lat();
    const auto phi2   = util::DEGREE_TO_RADIAN * Q2.lat();
    const auto lambda = util::DEGREE_TO_RADIAN * PointLonLat::normalise_angle_to_minimum(Q1.lon() - Q2.lon(), -180.);

    const auto cp1 = std::cos(phi1);
    const auto sp1 = std::sin(phi1);
    const auto cp2 = std::cos(phi2);
    const auto sp2 = std::sin(phi2);
    const auto cl  = std::cos(lambda);
    const auto sl  = std::sin(lambda);

    auto squared = [](double x) { return x * x; };

    const auto angle =
        std::atan2(std::sqrt(squared(cp2 * sl) + squared(cp1 * sp2 - sp1 * cp2 * cl)), sp1 * sp2 + cp1 * cp2 * cl);

    if (types::is_approximately_equal(angle, 0.)) {
        return 0.;
    }

    ASSERT(angle > 0.);
    return angle;
}


double Sphere::centralAngle(double radius, const PointXYZ& P1, const PointXYZ& P2) {
    ASSERT(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const auto d2 = PointXYZ::distance2(P1, P2);
    if (types::is_approximately_equal(d2, 0.)) {
        return 0.;
    }

    const auto chord = std::sqrt(d2) / radius;
    const auto angle = std::asin(chord * 0.5) * 2.;

    return angle;
}


double Sphere::distance(double radius, const PointLonLat& P1, const PointLonLat& P2) {
    return radius * centralAngle(P1, P2);
}


double Sphere::distance(double radius, const PointXYZ& P1, const PointXYZ& P2) {
    return radius * centralAngle(radius, P1, P2);
}


double Sphere::greatCircleLatitudeGivenLongitude(const PointLonLat& P1, const PointLonLat& P2, double Clon) {
    GreatCircle gc(P1, P2);
    auto lat = gc.latitude(Clon);
    return lat.size() == 1 ? lat[0] : std::numeric_limits<double>::signaling_NaN();
}


void Sphere::greatCircleLongitudeGivenLatitude(const PointLonLat& P1, const PointLonLat& P2, double Clat, double& Clon1,
                                               double& Clon2) {
    GreatCircle gc(P1, P2);
    auto lon = gc.longitude(Clat);

    Clon1 = lon.size() > 0 ? lon[0] : std::numeric_limits<double>::signaling_NaN();
    Clon2 = lon.size() > 1 ? lon[1] : std::numeric_limits<double>::signaling_NaN();
}


}  // namespace eckit::geo::figure
