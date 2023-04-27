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


namespace grit::types {


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

    PointXY(T x, T y) : P{x, y} {}
    PointXY(const PointXY& other) : P(other) {}
    PointXY(PointXY&& other) : P(other) {}

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
        return std::abs(x - other.x) < eps && std::abs(y - other.y) < eps;
    };

    // -- Members

    T& x = P::operator[](0);
    T& y = P::operator[](1);

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXY& p) {
        return out << '{' << p.x << ", " << p.y << '}';
    }
};


}  // namespace grit::types
