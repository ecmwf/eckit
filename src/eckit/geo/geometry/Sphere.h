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

#include "eckit/geo/types.h"


namespace eckit::geo::geometry {


struct Sphere {
    /// Great-circle central angle between two points (latitude/longitude coordinates) in radians
    static double angle(const PointLatLon&, const PointLatLon&);

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    static double angle(double radius, const PointXYZ&, const PointXYZ&);

    /// Great-circle distance between two points (latitude/longitude coordinates) in metres
    static double distance(double radius, const PointLatLon&, const PointLatLon&);

    /// Great-circle distance between two points (Cartesian coordinates) in metres
    static double distance(double radius, const PointXYZ&, const PointXYZ&);

    /// Surface area in square metres
    static double area(double radius);

    // Convert spherical coordinates to Cartesian
    static PointXYZ ll_to_xyz(double radius, const PointLatLon&, double height);

    // Convert Cartesian coordinates to spherical
    static PointLatLon xyz_to_ll(double radius, const PointXYZ&);
};


}  // namespace eckit::geo::geometry
