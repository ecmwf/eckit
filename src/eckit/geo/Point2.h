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

#include <cassert>

#include "eckit/geo/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
class Value;
}

//------------------------------------------------------------------------------------------------------

namespace eckit::geo {

//------------------------------------------------------------------------------------------------------

class Point2 final : public KPoint<2> {
    using BasePoint = KPoint<2>;

public:
    Point2() = default;

    Point2(const BasePoint& p) :
        BasePoint(p) {}

    explicit Point2(const double* p) :
        BasePoint(p) {}

    Point2(double x, double y) {
        x_[XX] = x;
        x_[YY] = y;
    }

    Point2(const Point2& other) :
        Point2(other.X, other.Y) {}

    Point2(Point2&& other) :
        Point2(other.X, other.Y) {}

    ~Point2() = default;

    double x() const { return x_[XX]; }

    double y() const { return x_[YY]; }

    double& X = x_[XX];

    double& Y = x_[YY];

    double x(size_t axis) const { return KPoint<2>::x(axis); }

    Point2& operator=(const Point2& other) {
        x_[0] = other[0];
        x_[1] = other[1];
        return *this;
    }

    Point2& operator=(Point2&& other) {
        x_[0] = other[0];
        x_[1] = other[1];
        return *this;
    }

    double operator[](const size_t& i) const {
        assert(i < 2);
        return x_[i];
    }

    double& operator[](const size_t& i) {
        assert(i < 2);
        return x_[i];
    }

    template <typename T>
    void assign(const T& p) {
        x_[XX] = p[XX];
        x_[YY] = p[YY];
    }

    explicit operator eckit::Value() const;

    static Point2 middle(const Point2& p, const Point2& q) { return (p + q) * 0.5; }
};

//------------------------------------------------------------------------------------------------------

bool points_equal(const Point2&, const Point2&, double eps = 1.e-9);

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo
