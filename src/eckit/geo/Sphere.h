/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once


namespace eckit::geo {


class Point3;
class PointLonLat;


struct Sphere {
    /// Great-circle central angle between two points in radians
    static double centralAngle(const PointLonLat&, const PointLonLat&);

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    static double centralAngle(double radius, const Point3&, const Point3&);

    /// Great-circle distance between two points in metres
    static double distance(double radius, const PointLonLat&, const PointLonLat&);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distance(double radius, const Point3&, const Point3&);

    /// Surface area in square metres
    static double area(double radius);

    /// Surface area between parallels and meridians defined by two points in square metres
    static double area(double radius, const PointLonLat&, const PointLonLat&);

    // Great-circle intermediate latitude provided two circle points and intermediate longitude in degrees
    static double greatCircleLatitudeGivenLongitude(const PointLonLat&, const PointLonLat&, double lon);

    // Great-circle intermediate longitude(s) provided two circle points and intermediate latitude in degrees
    static void greatCircleLongitudeGivenLatitude(const PointLonLat&, const PointLonLat&, double lat, double& lon1,
                                                  double& lon2);

    /// Convert spherical to Cartesian coordinates
    static Point3 convertSphericalToCartesian(double radius, const PointLonLat&, double height = 0.);

    /// Convert Cartesian to spherical coordinates
    static PointLonLat convertCartesianToSpherical(double radius, const Point3&);
};


}  // namespace eckit::geo
