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


namespace grit {


using Point2 = std::array<double, 2>;


struct PointLatLon : std::array<double, 2> {
    PointLatLon(double lat, double lon);

    double lat() const { return operator[](0); }
    double lon() const { return operator[](1); }
};


struct Point3 : std::array<double, 3> {
    static double distance2(const Point3& a, const Point3& b) {
        return (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]);
    }
};


}  // namespace grit
