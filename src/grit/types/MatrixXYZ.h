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

#include "grit/types/PointXYZ.h"


namespace grit::types {


template <typename T>
class MatrixXYZ final : protected std::array<T, 9> {
private:
    // -- Types

    using P = std::array<T, 9>;

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    MatrixXYZ() : P{1, 0, 0, 0, 1, 0, 0, 0, 1} {}
    MatrixXYZ(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) : P{xx, xy, xz, yx, yy, yz, zx, zy, zz} {}
    MatrixXYZ(const MatrixXYZ& other) : P(other) {}
    MatrixXYZ(MatrixXYZ&& other) : P(other) {}

    // -- Destructor

    ~MatrixXYZ() = default;

    // -- Convertors
    // None

    // -- Operators

    MatrixXYZ& operator=(const MatrixXYZ<T>& other) {
        P::operator=(other);
        return *this;
    }

    MatrixXYZ& operator=(MatrixXYZ<T>&& other) {
        P::operator=(other);
        return *this;
    }

    PointXYZ<T> operator*(const PointXYZ<T>& p) const {
        return {xx * p.x + xy * p.y + xz * p.z, yx * p.x + yy * p.y + yz * p.z, zx * p.x + zy * p.y + zz * p.z};
    }

    // -- Members

    T& xx = P::operator[](0);
    T& xy = P::operator[](1);
    T& xz = P::operator[](2);
    T& yx = P::operator[](3);
    T& yy = P::operator[](4);
    T& yz = P::operator[](5);
    T& zx = P::operator[](6);
    T& zy = P::operator[](7);
    T& zz = P::operator[](8);

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const MatrixXYZ& m) {
        return out << "{{" << m.xx << ", " << m.xy << ", " << m.xz << "}, {" << m.yx << ", " << m.yy << ", " << m.yz
                   << "}, {" << m.zx << ", " << m.zy << ", " << m.zz << "}}";
    }
};


}  // namespace grit::types
