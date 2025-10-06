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

#include "eckit/geometry/Sphere.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

/// Definition of a sphere parametrised with a geodetic datum
template <class DATUM>
struct SphereT {

    /// Sphere radius in metres
    inline static double radius() { return DATUM::radius(); }

    /// Great-circle central angle between two points (longitude/latitude coordinates) in radians
    inline static double centralAngle(const Point2& Alonlat, const Point2& Blonlat, bool normalise_angle = false) {
        return Sphere::centralAngle(Alonlat, Blonlat, normalise_angle);
    }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B) {
        return Sphere::centralAngle(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (longitude/latitude coordinates) in metres
    inline static double distance(const Point2& Alonlat, const Point2& Blonlat) {
        return Sphere::distance(DATUM::radius(), Alonlat, Blonlat);
    }

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    inline static double distance(const Point3& A, const Point3& B) { return Sphere::distance(DATUM::radius(), A, B); }

    /// Surface area in square metres
    inline static double area() { return Sphere::area(DATUM::radius()); }

    /// Surface area between parallels and meridians defined by two points (longitude/latitude coordinates) in square
    /// metres
    inline static double area(const Point2& WestNorth, const Point2& EastSouth) {
        return Sphere::area(DATUM::radius(), WestNorth, EastSouth);
    }

    // Great-circle intermediate latitude provided two circle points (A, B) and intermediate longitude (C) in degrees
    inline static double greatCircleLatitudeGivenLongitude(const Point2& Alonlat, const Point2& Blonlat,
                                                           const double& Clon) {
        return Sphere::greatCircleLatitudeGivenLongitude(Alonlat, Blonlat, Clon);
    }

    // Great-circle intermediate longitude(s) provided two circle points (A, B) and intermediate latitude (C) in degrees
    inline static void greatCircleLongitudeGivenLatitude(const Point2& Alonlat, const Point2& Blonlat,
                                                         const double& Clat, double& Clon1, double& Clon2) {
        return Sphere::greatCircleLongitudeGivenLatitude(Alonlat, Blonlat, Clat, Clon1, Clon2);
    }

    // Convert spherical coordinates to Cartesian
    inline static void convertSphericalToCartesian(const Point2& Alonlat, Point3& B, double height = 0.,
                                                   bool normalise_angle = false) {
        Sphere::convertSphericalToCartesian(DATUM::radius(), Alonlat, B, height, normalise_angle);
    }

    // Convert Cartesian coordinates to spherical
    inline static void convertCartesianToSpherical(const Point3& A, Point2& Blonlat) {
        Sphere::convertCartesianToSpherical(DATUM::radius(), A, Blonlat);
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
