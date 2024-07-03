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
 * @brief The Point3 class
 * @details A point on three-dimensional space, in (X, Y, Z) coordinates, linear in space.
 */
class Point3 final : protected std::array<double, 3> {
public:
    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    Point3() : Point3(0., 0., 0.) {}

    Point3(value_type x, value_type y, value_type z) : container_type{x, y, z} {}

    Point3(const Point3& other) : container_type(other) {}

    Point3(Point3&& other) : container_type(other) {}

    // -- Destructor

    ~Point3() = default;

    // -- Operators

    using container_type::operator[];

    Point3& operator=(const Point3& other) {
        container_type::operator=(other);
        return *this;
    }

    Point3& operator=(Point3&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    const value_type& X = container_type::operator[](0);
    const value_type& Y = container_type::operator[](1);
    const value_type& Z = container_type::operator[](2);

    // -- Methods

    static value_type distance(const Point3& p, const Point3& q, size_t axis) { return p.distance(q, axis); }
    static value_type distance(const Point3& p, const Point3& q) { return p.distance(q); }
    static value_type distance2(const Point3& p, const Point3& q) { return p.distance2(q); }

    value_type distance(const Point3&, size_t axis) const;
    value_type distance(const Point3&) const;
    value_type distance2(const Point3&) const;

    value_type x(size_t axis) const { return container_type::operator[](axis); }

    // -- Class members

    static constexpr size_t DIMS    = 3;
    static constexpr value_type EPS = 1e-9;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Point3& p) {
        return out << '{' << p.X << ", " << p.Y << ", " << p.Z << '}';
    }

    friend Point3 operator-(const Point3& p, const Point3& q) { return {p.X - q.X, p.Y - q.Y, p.Z - q.Z}; }
    friend Point3 operator+(const Point3& p, const Point3& q) { return {p.X + q.X, p.Y + q.Y, p.Z + q.Z}; }
    friend Point3 operator*(const Point3& p, value_type d) { return {p.X * d, p.Y * d, p.Z * d}; }

    friend bool operator==(const Point3& p, const Point3& q) { return p.X == q.X && p.Y == q.Y && p.Z == q.Z; }
    friend bool operator!=(const Point3& p, const Point3& q) { return !operator==(p, q); }
};


bool points_equal(const Point3&, const Point3&, Point3::value_type eps = Point3::EPS);


}  // namespace eckit::geo
