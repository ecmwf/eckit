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
 * @brief The PointXYZ class
 * @details A point on three-dimensional space, in (X, Y, Z) coordinates, linear in space.
 */
class PointXYZ final : protected std::array<double, 3> {
public:

    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    PointXYZ() : PointXYZ(0., 0., 0.) {}

    PointXYZ(value_type x, value_type y, value_type z) : container_type{x, y, z} {}

    PointXYZ(const PointXYZ& other) : container_type(other) {}

    PointXYZ(PointXYZ&& other) : container_type(other) {}

    // -- Destructor

    ~PointXYZ() = default;

    // -- Operators

    using container_type::operator[];

    PointXYZ& operator=(const PointXYZ& other) {
        container_type::operator=(other);
        return *this;
    }

    PointXYZ& operator=(PointXYZ&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    const value_type& X = container_type::operator[](0);
    const value_type& Y = container_type::operator[](1);
    const value_type& Z = container_type::operator[](2);

    // -- Methods

    using container_type::data;

    static size_t dimensions() { return DIMS; }

    static value_type distance(const PointXYZ& p, const PointXYZ& q, size_t axis) { return p.distance(q, axis); }
    static value_type distance(const PointXYZ& p, const PointXYZ& q) { return p.distance(q); }
    static value_type distance2(const PointXYZ& p, const PointXYZ& q) { return p.distance2(q); }

    value_type distance(const PointXYZ&, size_t axis) const;
    value_type distance(const PointXYZ&) const;
    value_type distance2(const PointXYZ&) const;

    value_type x(size_t axis) const { return container_type::operator[](axis); }

    // -- Class members

    static constexpr size_t DIMS    = 3;
    static constexpr value_type EPS = 1e-9;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXYZ& p) {
        return out << '{' << p.X << ", " << p.Y << ", " << p.Z << '}';
    }

    friend PointXYZ operator-(const PointXYZ& p, const PointXYZ& q) { return {p.X - q.X, p.Y - q.Y, p.Z - q.Z}; }
    friend PointXYZ operator+(const PointXYZ& p, const PointXYZ& q) { return {p.X + q.X, p.Y + q.Y, p.Z + q.Z}; }
    friend PointXYZ operator*(const PointXYZ& p, value_type d) { return {p.X * d, p.Y * d, p.Z * d}; }

    friend bool operator==(const PointXYZ& p, const PointXYZ& q) { return p.X == q.X && p.Y == q.Y && p.Z == q.Z; }
    friend bool operator!=(const PointXYZ& p, const PointXYZ& q) { return !operator==(p, q); }

    friend value_type dot(const PointXYZ& p, const PointXYZ& q) { return p.X * q.X + p.Y * q.Y + p.Z * q.Z; }

    friend PointXYZ cross(const PointXYZ& p, const PointXYZ& q) {
        return {p.Y * q.Z - p.Z * q.Y, p.Z * q.X - p.X * q.Z, p.X * q.Y - p.Y * q.X};
    }
};


bool points_equal(const PointXYZ&, const PointXYZ&, PointXYZ::value_type eps = PointXYZ::EPS);


}  // namespace eckit::geo
