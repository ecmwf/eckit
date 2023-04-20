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

    PointXYZ(T x, T y, T z) : P{x, y, z} {}
    PointXYZ(const PointXYZ<T>& other) : P(other) {}
    PointXYZ(PointXYZ<T>&& other) : P(other) {}

    // -- Destructor

    ~PointXYZ() = default;

    // -- Convertors
    // None

    // -- Operators

    PointXYZ& operator=(const PointXYZ<T>& other) {
        P::operator=(other);
        return *this;
    }

    PointXYZ& operator=(PointXYZ<T>&& other) {
        P::operator=(other);
        return *this;
    }


    // -- Members

    T& x = P::operator[](0);
    T& y = P::operator[](1);
    T& z = P::operator[](2);

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static T distance2(const PointXYZ<T>& a, const PointXYZ<T>& b) {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
    }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointXYZ& p) {
        return out << '{' << p.x << ", " << p.y << ", " << p.z << '}';
    }
};


}  // namespace grit::types
