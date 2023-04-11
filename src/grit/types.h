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

#include <array>
#include <vector>


namespace grit {


struct PointLatLon : std::array<double, 2> {
    PointLatLon(double lat, double lon);

    double& lat = operator[](0);
    double& lon = operator[](1);
};


struct Point3 : std::array<double, 3> {
    Point3(double x, double y, double z) : array{x, y, z} {}

    static double distance2(const Point3& a, const Point3& b) {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
    }

    double& x = operator[](0);
    double& y = operator[](1);
    double& z = operator[](2);
};


using pl_type = std::vector<long>;


}  // namespace grit
