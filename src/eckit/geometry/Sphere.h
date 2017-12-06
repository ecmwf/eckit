/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Sphere_H
#define Sphere_H

#include "eckit/exception/Exceptions.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

class Point2;
class Point3;

//------------------------------------------------------------------------------------------------------

struct Sphere {

    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    static double centralAngle(const Point2& Alatlon, const Point2& Blatlon);

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    static double centralAngle(const double& radius, const Point3& A, const Point3& B);

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    static double distanceInMeters(const double& radius, const Point2& Alatlon, const Point2& Blatlon);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distanceInMeters(const double& radius, const Point3& A, const Point3& B);

    // Great-circle intermediate position provided two circle points (A, B) and longitude (C) in degrees
    static void greatCircleLatitudeGivenLongitude(const Point2& Alatlon, const Point2& Blatlon, Point2& Clatlon);

    // Convert spherical coordinates to Cartesian
    static void convertSphericalToCartesian(const double& radius, const Point2& Alatlon, Point3& B, double height = 0.);

    // Convert Cartesian coordinates to spherical
    static void convertCartesianToSpherical(const double& radius, const Point3& A, Point2& Blatlon);

};

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif

