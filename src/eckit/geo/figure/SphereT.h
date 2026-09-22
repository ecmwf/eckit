// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/figure/Sphere.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo::figure {


/// Sphere parametrised with a geodetic datum
template <class DATUM>
class SphereT : public Figure {
public:

    // -- Constructors

    SphereT() = default;

    // -- Overridden methods

    double R() const override { return DATUM::radius; }
    double a() const override { return DATUM::radius; }
    double b() const override { return DATUM::radius; }

    bool is_default() const override { return DATUM::default_figure; }

    /// Surface area [L^2]
    double area() const override { return _area(); }

    /// Surface area between parallels and meridians [L^2]
    double area(const area::BoundingBox& bbox) const override { return _area(bbox); }

    // -- Class methods

    /// Sphere radius
    inline static double radius() { return DATUM::radius; }

    /// Great-circle central angle between two points [radian]
    inline static double centralAngle(const PointLonLat& A, const PointLonLat& B) { return Sphere::centralAngle(A, B); }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const PointXYZ& A, const PointXYZ& B) {
        return Sphere::centralAngle(DATUM::radius, A, B);
    }

    /// Great-circle distance between two points
    inline static double distance(const PointLonLat& A, const PointLonLat& B) {
        return Sphere::distance(DATUM::radius, A, B);
    }

    /// Great-circle distance between two points (Cartesian coordinates)
    inline static double distance(const PointXYZ& A, const PointXYZ& B) {
        return Sphere::distance(DATUM::radius, A, B);
    }

    /// Surface area [L^2]
    inline static double _area() { return Sphere::area(DATUM::radius); }

    /// Surface area between parallels and meridians [L^2]
    inline static double _area(const area::BoundingBox& bbox) { return Sphere::area(DATUM::radius, bbox); }

    /// Convert spherical to Cartesian coordinates
    inline static PointXYZ _convertSphericalToCartesian(const PointLonLat& P, double height = 0.) {
        return Sphere::convertSphericalToCartesian(DATUM::radius, P, height);
    }

    /// Convert Cartesian to spherical coordinates
    inline static PointLonLat _convertCartesianToSpherical(const PointXYZ& P) {
        return Sphere::convertCartesianToSpherical(DATUM::radius, P);
    }
};


}  // namespace eckit::geo::figure
