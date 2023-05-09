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
#include <cmath>
#include <cstddef>
#include <ostream>


namespace eckit::geo::types {


template <typename T>
class PointXYZ final : protected std::array<T, 3> {
private:
    // -- Types

    using P = std::array<T, 3>;

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    PointXYZ() :
        P{0, 0, 0} {}
    PointXYZ(T x, T y, T z) :
        P{x, y, z} {}
    PointXYZ(const PointXYZ& other) :
        P(other) {}
    PointXYZ(PointXYZ&& other) :
        P(other) {}

    // -- Destructor

    ~PointXYZ() = default;

    // -- Convertors
    // None

    // -- Operators

    PointXYZ& operator=(const PointXYZ& other) {
        P::operator=(other);
        return *this;
    }

    PointXYZ& operator=(PointXYZ&& other) {
        P::operator=(other);
        return *this;
    }

    bool is_approximately_equal(const PointXYZ& other, T eps) const {
        return std::abs(X - other.X) < eps && std::abs(Y - other.Y) < eps && std::abs(Z - other.Z) < eps;
    };

    // -- Members

    T& X = P::operator[](0);
    T& Y = P::operator[](1);
    T& Z = P::operator[](2);

    static constexpr size_t DIMS = 3;

    // -- Methods

    T x(size_t dim) const { return P::at(dim); }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static T distance(const PointXYZ& a, const PointXYZ& b, size_t dim) { return std::abs(a.x(dim) - b.x(dim)); }

    static T distance(const PointXYZ& a, const PointXYZ& b) { return std::sqrt(distance2(a, b)); }

    static T distance2(const PointXYZ& a, const PointXYZ& b) {
        return (a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y) + (a.Z - b.Z) * (a.Z - b.Z);
    }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXYZ& p) {
        return out << '{' << p.X << ", " << p.Y << ", " << p.Z << '}';
    }
};


}  // namespace eckit::geo::types
