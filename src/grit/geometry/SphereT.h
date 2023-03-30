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


#pragma once


#include "grit/geometry/Sphere.h"


namespace grit::geometry {


/// Definition of a sphere parametrised with a geodetic datum
template <class DATUM>
struct SphereT {

    /// Sphere radius in metres
    inline static double radius() { return DATUM::radius(); }

    /// Great-circle central angle between two points (longitude/latitude coordinates) in radians
    inline static double centralAngle(const PointLatLon& A, const PointLatLon& B) { return Sphere::centralAngle(A, B); }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B) {
        return Sphere::centralAngle(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (longitude/latitude coordinates) in metres
    inline static double distance(const PointLatLon& A, const PointLatLon& B) {
        return Sphere::distance(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    inline static double distance(const Point3& A, const Point3& B) { return Sphere::distance(DATUM::radius(), A, B); }

    /// Surface area in square metres
    inline static double area() { return Sphere::area(DATUM::radius()); }

    // Convert spherical coordinates to Cartesian
    inline static Point3 convertSphericalToCartesian(const PointLatLon& P, double height = 0.) {
        return Sphere::convertSphericalToCartesian(DATUM::radius(), P, height);
    }

    // Convert Cartesian coordinates to spherical
    inline static PointLatLon convertCartesianToSpherical(const Point3& P) {
        return Sphere::convertCartesianToSpherical(DATUM::radius(), P);
    }
};


}  // namespace grit::geometry
