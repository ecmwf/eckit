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
    static double centralAngle(double radius, const Point3&, const Point3&);

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    static double distance(double radius, const PointLatLon&, const PointLatLon&);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distance(double radius, const Point3&, const Point3&);

    /// Surface area in square metres
    static double area(double radius);

    // Convert spherical coordinates to Cartesian
    static Point3 convertSphericalToCartesian(double radius, const PointLatLon&, double height = 0.);

    // Convert Cartesian coordinates to spherical
    static PointLatLon convertCartesianToSpherical(double radius, const Point3&);
};


}  // namespace grit::geometry
