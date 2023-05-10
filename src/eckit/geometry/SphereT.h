/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SphereT_H
#define SphereT_H

#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"
#include "eckit/geometry/Sphere.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

/// Definition of a sphere parametrised with a geodetic datum
template <class DATUM>
struct SphereT {

    /// Sphere radius in metres
    inline static double radius() { return DATUM::radius(); }

    /// Great-circle central angle between two points (longitude/latitude coordinates) in radians
    inline static double centralAngle(const PointLonLat& A, const PointLonLat& B) {
        return Sphere::centralAngle(A, B);
    }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B) {
        return Sphere::centralAngle(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (longitude/latitude coordinates) in metres
    inline static double distance(const PointLonLat& A, const PointLonLat& B) {
        return Sphere::distance(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    inline static double distance(const Point3& A, const Point3& B) { return Sphere::distance(DATUM::radius(), A, B); }

    /// Surface area in square metres
    inline static double area() { return Sphere::area(DATUM::radius()); }

    /// Surface area between parallels and meridians defined by two points (longitude/latitude coordinates) in square metres
    inline static double area(const PointLonLat& WestNorth, const PointLonLat& EastSouth) {
        return Sphere::area(DATUM::radius(), WestNorth, EastSouth);
    }

    /// Great-circle intermediate latitude provided two circle points (A, B) and intermediate longitude (C) in degrees
    inline static double greatCircleLatitudeGivenLongitude(const PointLonLat& A,
                                                           const PointLonLat& B,
                                                           double Clon) {
        return Sphere::greatCircleLatitudeGivenLongitude(A, B, Clon);
    }

    /// Great-circle intermediate longitude(s) provided two circle points (A, B) and intermediate latitude (C) in degrees
    inline static void greatCircleLongitudeGivenLatitude(
        const PointLonLat& A, const PointLonLat& B, double Clat, double& Clon1, double& Clon2) {
        return Sphere::greatCircleLongitudeGivenLatitude(A, B, Clat, Clon1, Clon2);
    }

    /// Convert spherical coordinates to Cartesian
    inline static Point3 convertSphericalToCartesian(const PointLonLat& A, double height = 0.) {
        return Sphere::convertSphericalToCartesian(DATUM::radius(), A, height);
    }

    /// Convert Cartesian coordinates to spherical
    inline static PointLonLat convertCartesianToSpherical(const Point3& A) {
        return Sphere::convertCartesianToSpherical(DATUM::radius(), A);
    }
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
