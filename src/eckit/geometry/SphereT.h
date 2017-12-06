/*
 * (C) Copyright 1996-2017 ECMWF.
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

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

/// Definition of a sphere parametrised with a geodetic datum
template< class DATUM >
struct SphereT {

    /// Sphere radius in metres
    inline static double radiusInMeters()
    {
        return DATUM::radiusInMeters();
    }

    /// Great-circle central angle between two points (longitude/latitude coordinates) in radians
    inline static double centralAngle(const Point2& Alonlat, const Point2& Blonlat)
    {
        return Sphere::centralAngle(Alonlat, Blonlat);
    }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B)
    {
        return Sphere::centralAngle(DATUM::radiusInMeters(), A, B);
    }

    /// Great-circle distance between two points (longitude/latitude coordinates) in metres
    inline static double distanceInMeters(const Point2& Alonlat, const Point2& Blonlat)
    {
        return Sphere::distanceInMeters(DATUM::radiusInMeters(), Alonlat, Blonlat);
    }

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    inline static double distanceInMeters(const Point3& A, const Point3& B)
    {
        return Sphere::distanceInMeters(DATUM::radiusInMeters(), A, B);
    }

    /// Surface area in square metres
    inline static double areaInSqMeters()
    {
        return Sphere::areaInSqMeters(DATUM::radiusInMeters());
    }

    /// Surface area between parallels and meridians defined by two points (longitude/latitude coordinates) in square metres
    inline static double areaInSqMeters(const Point2& WestNorth, const Point2& EastSouth)
    {
        return Sphere::areaInSqMeters(DATUM::radiusInMeters(), WestNorth, EastSouth);
    }

    // Great-circle intermediate position provided two circle points (A, B) and longitude (C) in degrees
    inline static void greatCircleLatitudeGivenLongitude(const Point2& Alonlat, const Point2& Blonlat, Point2& Clonlat)
    {
        Sphere::greatCircleLatitudeGivenLongitude(Alonlat, Blonlat, Clonlat);
    }

    // Convert spherical coordinates to Cartesian
    inline static void convertSphericalToCartesian(const Point2& Alonlat, Point3& B, double height = 0.)
    {
        Sphere::convertSphericalToCartesian(DATUM::radiusInMeters(), Alonlat, B, height);
    }

    // Convert Cartesian coordinates to spherical
    inline static void convertCartesianToSpherical(const Point3& A, Point2& Blonlat)
    {
        Sphere::convertCartesianToSpherical(DATUM::radiusInMeters(), A, Blonlat);
    }

};

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

