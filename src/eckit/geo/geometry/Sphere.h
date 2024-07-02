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
namespace area {
class BoundingBox;
}
}  // namespace eckit::geo


namespace eckit::geo::geometry {


/// Spherical geometry
struct Sphere {
    /// Great-circle central angle between two points [radian]
    static double centralAngle(const PointLonLat&, const PointLonLat&);

    /// Great-circle central angle between two points (Cartesian coordinates) [m]
    static double centralAngle(double radius, const Point3&, const Point3&);

    /// Great-circle distance between two points [m]
    static double distance(double radius, const PointLonLat&, const PointLonLat&);

    /// Great-circle distance between two points (Cartesian coordinates) [m]
    static double distance(double radius, const Point3&, const Point3&);

    /// Surface area [m ** 2]
    static double area(double radius);

    /// Surface area between parallels and meridians [m ** 2]
    static double area(double radius, const area::BoundingBox&);

    /// Great-circle intermediate latitude provided two circle points and intermediate longitude [degree]
    static double greatCircleLatitudeGivenLongitude(const PointLonLat&, const PointLonLat&, double lon);

    /// Great-circle intermediate longitude(s) provided two circle points and intermediate latitude [degree]
    static void greatCircleLongitudeGivenLatitude(const PointLonLat&, const PointLonLat&, double lat, double& lon1,
                                                  double& lon2);

    /// Convert spherical to Cartesian coordinates
    static Point3 convertSphericalToCartesian(double radius, const PointLonLat&, double height = 0.);

    /// Convert Cartesian to spherical coordinates
    static PointLonLat convertCartesianToSpherical(double radius, const Point3&);
};


}  // namespace eckit::geo::geometry
