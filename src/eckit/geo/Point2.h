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

#include <array>
#include <ostream>


namespace eckit::geo {


class Point2 final : protected std::array<double, 2> {
private:
    // -- Types

    using container_type = std::array<double, 2>;
    using container_type::value_type;

public:
    // -- Constructors

    Point2() :
        Point2(0., 0.) {}

    Point2(double x, double y) :
        container_type{x, y} {}

    Point2(const Point2& other) :
        container_type(other) {}

    Point2(Point2&& other) :
        container_type(other) {}

    // -- Destructor

    ~Point2() = default;

    // -- Operators

    using container_type::operator[];

    Point2& operator=(const Point2& other) {
        container_type::operator=(other);
        return *this;
    }

    Point2& operator=(Point2&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    double& X = container_type::operator[](0);
    double& Y = container_type::operator[](1);

    // -- Methods

    static size_t dimensions() { return DIMS; }

    static double norm(const Point2& p) { return p.norm(); }
    static Point2 normalize(const Point2& p) { return p.normalize(); }
    static Point2 middle(const Point2& p, const Point2& q) { return p.middle(q); }
    static double distance(const Point2& p, const Point2& q, size_t axis) { return p.distance(q, axis); }
    static double distance(const Point2& p, const Point2& q) { return p.distance(q); }
    static double distance2(const Point2& p, const Point2& q) { return p.distance2(q); }

    double norm() const;
    Point2 normalize() const;
    Point2 middle(const Point2&) const;
    double distance(const Point2&, size_t axis) const;
    double distance(const Point2&) const;
    double distance2(const Point2&) const;

    double x(size_t axis) const { return container_type::operator[](axis); }

    // -- Class members

    static constexpr size_t DIMS = 2;
    static constexpr double EPS  = 1e-9;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Point2& p) {
        return out << '{' << p.X << ", " << p.Y << '}';
    }

    friend Point2 operator-(const Point2& p, const Point2& q) { return {p.X - q.X, p.Y - q.Y}; }
    friend Point2 operator+(const Point2& p, const Point2& q) { return {p.X + q.X, p.Y + q.Y}; }
    friend Point2 operator*(const Point2& p, double d) { return {p.X * d, p.Y * d}; }

    friend bool operator==(const Point2& p, const Point2& q) { return p.X == q.X && p.Y == q.Y; }
    friend bool operator!=(const Point2& p, const Point2& q) { return !operator==(p, q); }
};


bool points_equal(const Point2&, const Point2&, double eps = Point2::EPS);


}  // namespace eckit::geo
