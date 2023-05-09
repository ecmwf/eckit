/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/geometry/Sphere.h"

#include <algorithm>
#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/geometry/Spheroid.h"
#include "eckit/geo/util.h"


namespace eckit::geo::geometry {


inline double squared(double x) {
    return x * x;
}


double Sphere::angle(const PointLatLon& A, const PointLatLon& B) {
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

    const double phi1   = util::degrees_to_radians * A.lat;
    const double phi2   = util::degrees_to_radians * B.lat;
    const double lambda = util::degrees_to_radians * (B.lon - A.lon);

    const double cos_phi1   = std::cos(phi1);
    const double sin_phi1   = std::sin(phi1);
    const double cos_phi2   = std::cos(phi2);
    const double sin_phi2   = std::sin(phi2);
    const double cos_lambda = std::cos(lambda);
    const double sin_lambda = std::sin(lambda);

    const double sigma = atan2(
        std::sqrt(squared(cos_phi2 * sin_lambda) + squared(cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_lambda)),
        sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_lambda);

    if (util::is_approximately_equal(sigma, 0.)) {
        return 0.;
    }

    ASSERT(sigma > 0.);
    return sigma;
}


double Sphere::angle(double radius, const PointXYZ& A, const PointXYZ& B) {
    ASSERT(radius > 0.);

    // Δσ = 2 * asin( chord / 2 )

    const double d2 = PointXYZ::distance2(A, B);
    if (util::is_approximately_equal(d2, 0.)) {
        return 0.;
    }

    const double chord = std::sqrt(d2) / radius;
    const double sigma = std::asin(chord * 0.5) * 2.;

    ASSERT(sigma > 0.);
    return sigma;
}


double Sphere::distance(double radius, const PointLatLon& A, const PointLatLon& B) {
    return radius * angle(A, B);
}


double Sphere::distance(double radius, const PointXYZ& A, const PointXYZ& B) {
    return radius * angle(radius, A, B);
}


double Sphere::area(double radius) {
    ASSERT(radius > 0.);
    return 4. * M_PI * radius * radius;
}


PointXYZ Sphere::ll_to_xyz(double radius, const PointLatLon& P, double height) {
    return Spheroid::ll_to_xyz(radius, radius, P, height);
}


PointLatLon Sphere::xyz_to_ll(double radius, const PointXYZ& A) {
    ASSERT(radius > 0.);

    // numerical conditioning for both z (poles) and y

    const double x = A.X;
    const double y = util::is_approximately_equal(A.Y, 0.) ? 0. : A.Y;
    const double z = std::min(radius, std::max(-radius, A.Z)) / radius;

    return {util::radians_to_degrees * std::asin(z), util::radians_to_degrees * std::atan2(y, x)};
}


}  // namespace eckit::geo::geometry
