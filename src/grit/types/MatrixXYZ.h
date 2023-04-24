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

#include "grit/exception.h"
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

    MatrixXYZ<T> operator*(const MatrixXYZ<T>& M) const {
        return {
            xx * M.xx + xy * M.yx + xz * M.zx, xx * M.xy + xy * M.yy + xz * M.zy, xx * M.xz + xy * M.yz + xz * M.zz,
            yx * M.xx + yy * M.yx + yz * M.zx, yx * M.xy + yy * M.yy + yz * M.zy, yx * M.xz + yy * M.yz + yz * M.zz,
            zx * M.xx + zy * M.yx + zz * M.zx, zx * M.xy + zy * M.yy + zz * M.zy, zx * M.xz + zy * M.yz + zz * M.zz};
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

    static MatrixXYZ<T> identity() { return {1, 0, 0, 0, 1, 0, 0, 0, 1}; }

    MatrixXYZ<T> inverse() const {
        auto det = xx * (yy * zz - yz * zy) - xy * (yx * zz - yz * zx) + xz * (yx * zy - yy * zx);
        ASSERT_MSG(det != 0, "MatrixXYZ: singular matrix");

        return {(yy * zz - yz * zy) / det, (xz * zy - xy * zz) / det, (xy * yz - xz * yy) / det,
                (yz * zx - yx * zz) / det, (xx * zz - xz * zx) / det, (xz * yx - xx * yz) / det,
                (yx * zy - yy * zx) / det, (xy * zx - xx * zy) / det, (xx * yy - xy * yx) / det};
    }

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
