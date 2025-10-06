/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Sphere_H
#define Sphere_H

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

class Point2;
class Point3;

//----------------------------------------------------------------------------------------------------------------------

struct Sphere {
    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    static double centralAngle(const Point2& Alonlat, const Point2& Blonlat, bool normalise_angle = false);

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    static double centralAngle(const double& radius, const Point3& A, const Point3& B);

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    static double distance(const double& radius, const Point2& Alonlat, const Point2& Blonlat);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distance(const double& radius, const Point3& A, const Point3& B);

    /// Surface area in square metres
    static double area(const double& radius);

    /// Surface area between parallels and meridians defined by two points (longitude/latitude
    /// coordinates) in square metres
    static double area(const double& radius, const Point2& Alonlat, const Point2& Blonlat);

    // Great-circle intermediate latitude provided two circle points (A, B) and intermediate
    // longitude (C) in degrees
    static double greatCircleLatitudeGivenLongitude(const Point2& Alonlat, const Point2& Blonlat, const double& Clon);

    // Great-circle intermediate longitude(s) provided two circle points (A, B) and intermediate
    // latitude (C) in degrees
    static void greatCircleLongitudeGivenLatitude(const Point2& Alonlat, const Point2& Blonlat, const double& Clat,
                                                  double& Clon1, double& Clon2);

    // Convert spherical coordinates to Cartesian
    static void convertSphericalToCartesian(const double& radius, const Point2& Alonlat, Point3& B, double height = 0.,
                                            bool normalise_angle = false);

    // Convert Cartesian coordinates to spherical
    static void convertCartesianToSpherical(const double& radius, const Point3& A, Point2& Blonlat);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
