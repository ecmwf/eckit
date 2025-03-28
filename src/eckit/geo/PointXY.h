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
 * @brief The PointXY class
 * @details A point on two-dimensional space, in (X, Y) coordinates, linear in space.
 */
class PointXY final : protected std::array<double, 2> {
public:

    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    PointXY() : PointXY(0., 0.) {}

    PointXY(value_type x, value_type y) : container_type{x, y} {}

    PointXY(const PointXY& other) : container_type(other) {}

    PointXY(PointXY&& other) : container_type(other) {}

    // -- Destructor

    ~PointXY() = default;

    // -- Operators

    using container_type::operator[];

    PointXY& operator=(const PointXY& other) {
        container_type::operator=(other);
        return *this;
    }

    PointXY& operator=(PointXY&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    const value_type& X = container_type::operator[](0);
    const value_type& Y = container_type::operator[](1);

    // -- Methods

    using container_type::data;

    static size_t dimensions() { return DIMS; }

    static value_type norm(const PointXY& p) { return p.norm(); }
    static PointXY normalize(const PointXY& p) { return p.normalize(); }
    static PointXY middle(const PointXY& p, const PointXY& q) { return p.middle(q); }
    static value_type distance(const PointXY& p, const PointXY& q, size_t axis) { return p.distance(q, axis); }
    static value_type distance(const PointXY& p, const PointXY& q) { return p.distance(q); }
    static value_type distance2(const PointXY& p, const PointXY& q) { return p.distance2(q); }

    value_type norm() const;
    PointXY normalize() const;
    PointXY middle(const PointXY&) const;
    value_type distance(const PointXY&, size_t axis) const;
    value_type distance(const PointXY&) const;
    value_type distance2(const PointXY&) const;

    value_type x(size_t axis) const { return container_type::operator[](axis); }

    // -- Class members

    static constexpr size_t DIMS    = 2;
    static constexpr value_type EPS = 1e-9;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXY& p) {
        return out << '{' << p.X << ", " << p.Y << '}';
    }

    friend PointXY operator-(const PointXY& p, const PointXY& q) { return {p.X - q.X, p.Y - q.Y}; }
    friend PointXY operator+(const PointXY& p, const PointXY& q) { return {p.X + q.X, p.Y + q.Y}; }
    friend PointXY operator*(const PointXY& p, value_type d) { return {p.X * d, p.Y * d}; }

    friend bool operator==(const PointXY& p, const PointXY& q) { return p.X == q.X && p.Y == q.Y; }
    friend bool operator!=(const PointXY& p, const PointXY& q) { return !operator==(p, q); }
};


bool points_equal(const PointXY&, const PointXY&, PointXY::value_type eps = PointXY::EPS);


}  // namespace eckit::geo
