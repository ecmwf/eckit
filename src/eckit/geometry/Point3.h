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

#include "eckit/geometry/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

class Point3 : public KPoint<3> {
    using BasePoint = KPoint<3>;

public:
    Point3() = default;

    Point3(const BasePoint& p) :
        BasePoint(p) {}

    explicit Point3(const double* p) :
        BasePoint(p) {}

    Point3(double x, double y, double z) {
        x_[XX] = x;
        x_[YY] = y;
        x_[ZZ] = z;
    }

    Point3(const Point3& other) :
        Point3(other.X, other.Y, other.Z) {}

    Point3(Point3&& other) :
        Point3(other.X, other.Y, other.Z) {}

    double& X = x_[XX];

    double& Y = x_[YY];

    double& Z = x_[ZZ];

    Point3& operator=(const Point3& other) {
        x_[0] = other[0];
        x_[1] = other[1];
        x_[2] = other[2];
        return *this;
    }

    Point3& operator=(Point3&& other) {
        x_[0] = other[0];
        x_[1] = other[1];
        x_[2] = other[2];
        return *this;
    }

    double operator[](const size_t& i) const {
        assert(i < 3);
        return x_[i];
    }
    double& operator[](const size_t& i) {
        assert(i < 3);
        return x_[i];
    }

    template <typename T>
    void assign(const T& p) {
        x_[XX] = p[XX];
        x_[YY] = p[YY];
        x_[ZZ] = p[ZZ];
    }

    static Point3 cross(const Point3& p1, const Point3& p2) {
        return Point3(p1[YY] * p2[ZZ] - p1[ZZ] * p2[YY], p1[ZZ] * p2[XX] - p1[XX] * p2[ZZ],
                      p1[XX] * p2[YY] - p1[YY] * p2[XX]);
    }
};

//------------------------------------------------------------------------------------------------------

bool points_equal(const Point3&, const Point3&);

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry
