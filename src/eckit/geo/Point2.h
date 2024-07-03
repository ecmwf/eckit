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


/**
 * @brief The Point2 class
 * @details A point on two-dimensional space, in (X, Y) coordinates, linear in space.
 */
class Point2 final : protected std::array<double, 2> {
public:
    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    Point2() : Point2(0., 0.) {}

    Point2(value_type x, value_type y) : container_type{x, y} {}

    Point2(const Point2& other) : container_type(other) {}

    Point2(Point2&& other) : container_type(other) {}

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

    const value_type& X = container_type::operator[](0);
    const value_type& Y = container_type::operator[](1);

    // -- Methods

    static size_t dimensions() { return DIMS; }

    static value_type norm(const Point2& p) { return p.norm(); }
    static Point2 normalize(const Point2& p) { return p.normalize(); }
    static Point2 middle(const Point2& p, const Point2& q) { return p.middle(q); }
    static value_type distance(const Point2& p, const Point2& q, size_t axis) { return p.distance(q, axis); }
    static value_type distance(const Point2& p, const Point2& q) { return p.distance(q); }
    static value_type distance2(const Point2& p, const Point2& q) { return p.distance2(q); }

    value_type norm() const;
    Point2 normalize() const;
    Point2 middle(const Point2&) const;
    value_type distance(const Point2&, size_t axis) const;
    value_type distance(const Point2&) const;
    value_type distance2(const Point2&) const;

    value_type x(size_t axis) const { return container_type::operator[](axis); }

    // -- Class members

    static constexpr size_t DIMS    = 2;
    static constexpr value_type EPS = 1e-9;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Point2& p) {
        return out << '{' << p.X << ", " << p.Y << '}';
    }

    friend Point2 operator-(const Point2& p, const Point2& q) { return {p.X - q.X, p.Y - q.Y}; }
    friend Point2 operator+(const Point2& p, const Point2& q) { return {p.X + q.X, p.Y + q.Y}; }
    friend Point2 operator*(const Point2& p, value_type d) { return {p.X * d, p.Y * d}; }

    friend bool operator==(const Point2& p, const Point2& q) { return p.X == q.X && p.Y == q.Y; }
    friend bool operator!=(const Point2& p, const Point2& q) { return !operator==(p, q); }
};


bool points_equal(const Point2&, const Point2&, Point2::value_type eps = Point2::EPS);


}  // namespace eckit::geo
