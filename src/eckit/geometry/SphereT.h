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
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

/// Definition of a sphere parametrised with a geodetic datum
template< class DATUM >
struct SphereT {

    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    inline static double centralAngle(const Point2& Alatlon, const Point2& Blatlon)
    {
        return Sphere::centralAngle(Alatlon, Blatlon);
    }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B)
    {
        return Sphere::centralAngle(DATUM::radiusInMeters(), A, B);
    }

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    inline static double distanceInMeters(const Point2& Alatlon, const Point2& Blatlon)
    {
        return Sphere::distanceInMeters(DATUM::radiusInMeters(), Alatlon, Blatlon);
    }

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    inline static double distanceInMeters(const Point3& A, const Point3& B)
    {
        return Sphere::distanceInMeters(DATUM::radiusInMeters(), A, B);
    }

    // Great-circle intermediate position provided two circle points (A, B) and longitude (C) in degrees
    inline static void greatCircleLatitudeGivenLongitude(const Point2& Alatlon, const Point2& Blatlon, Point2& Clatlon)
    {
        Sphere::greatCircleLatitudeGivenLongitude(Alatlon, Blatlon, Clatlon);
    }

    // Convert spherical coordinates to Cartesian
    inline static void convertSphericalToCartesian(const Point2& Alatlon, Point3& B, double height = 0.)
    {
        Sphere::convertSphericalToCartesian(DATUM::radiusInMeters(), Alatlon, B, height);
    }

    // Convert Cartesian coordinates to spherical
    inline static void convertCartesianToSpherical(const Point3& A, Point2& Blatlon)
    {
        Sphere::convertCartesianToSpherical(DATUM::radiusInMeters(), A, Blatlon);
    }

};

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

