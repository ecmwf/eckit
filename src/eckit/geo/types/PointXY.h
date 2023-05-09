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
#include <ostream>


namespace eckit::geo::types {


template <typename T>
class PointXY final : protected std::array<T, 2> {
private:
    // -- Types

    using P = std::array<T, 2>;

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    PointXY() :
        P{0, 0} {}
    PointXY(T x, T y) :
        P{x, y} {}
    PointXY(const PointXY& other) :
        P(other) {}
    PointXY(PointXY&& other) :
        P(other) {}

    // -- Destructor

    ~PointXY() = default;

    // -- Convertors
    // None

    // -- Operators

    PointXY& operator=(const PointXY& other) {
        P::operator=(other);
        return *this;
    }

    PointXY& operator=(PointXY&& other) {
        P::operator=(other);
        return *this;
    }

    bool is_approximately_equal(const PointXY& other, T eps) const {
        return std::abs(X - other.X) < eps && std::abs(Y - other.Y) < eps;
    };

    // -- Members

    T& X = P::operator[](0);
    T& Y = P::operator[](1);

    static constexpr size_t DIMS = 2;

    // -- Methods

    T x(size_t dim) const { return P::at(dim); }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static T distance(const PointXY& a, const PointXY& b, size_t dim) { return std::abs(a.x(dim) - b.x(dim)); }

    static T distance(const PointXY& a, const PointXY& b) { return std::sqrt(distance2(a, b)); }

    static T distance2(const PointXY& a, const PointXY& b) {
        return (a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y);
    }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXY& p) {
        return out << '{' << p.X << ", " << p.Y << '}';
    }
};


}  // namespace eckit::geo::types
