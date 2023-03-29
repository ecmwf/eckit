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

#include "grit/types.h"


namespace grit::geometry {


struct Sphere {
    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    static double centralAngle(const PointLatLon&, const PointLatLon&);

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    static double centralAngle(const double& radius, const Point3&, const Point3&);

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    static double distance(const double& radius, const PointLatLon&, const PointLatLon&);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distance(const double& radius, const Point3&, const Point3&);

    /// Surface area in square metres
    static double area(const double& radius);

    // Great-circle intermediate latitude provided two circle points (A, B) and intermediate
    // longitude (C) in degrees
    static double greatCircleLatitudeGivenLongitude(const PointLatLon&, const PointLatLon&, const double& lon);

    // Great-circle intermediate longitude(s) provided two circle points (A, B) and intermediate
    // latitude (C) in degrees
    static void greatCircleLongitudeGivenLatitude(const PointLatLon&, const PointLatLon&, const double& lat,
                                                  double& lon1, double& lon2);

    // Convert spherical coordinates to Cartesian
    static void convertSphericalToCartesian(const double& radius, const PointLatLon&, Point3&, double height = 0.);

    // Convert Cartesian coordinates to spherical
    static void convertCartesianToSpherical(const double& radius, const Point3&, PointLatLon&);
};


}  // namespace grit::geometry
