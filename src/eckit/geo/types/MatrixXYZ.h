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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/types/PointXYZ.h"


namespace eckit::geo::types {


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

    MatrixXYZ(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) :
        P{xx, xy, xz, yx, yy, yz, zx, zy, zz} {}
    MatrixXYZ(const MatrixXYZ& other) :
        P(other) {}
    MatrixXYZ(MatrixXYZ&& other) :
        P(other) {}

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
        return {XX * p.X + XY * p.Y + XZ * p.Z, YX * p.X + YY * p.Y + YZ * p.Z, ZX * p.X + ZY * p.Y + ZZ * p.Z};
    }

    MatrixXYZ<T> operator*(const MatrixXYZ<T>& M) const {
        return {
            XX * M.XX + XY * M.YX + XZ * M.ZX, XX * M.XY + XY * M.YY + XZ * M.ZY, XX * M.XZ + XY * M.YZ + XZ * M.ZZ,
            YX * M.XX + YY * M.YX + YZ * M.ZX, YX * M.XY + YY * M.YY + YZ * M.ZY, YX * M.XZ + YY * M.YZ + YZ * M.ZZ,
            ZX * M.XX + ZY * M.YX + ZZ * M.ZX, ZX * M.XY + ZY * M.YY + ZZ * M.ZY, ZX * M.XZ + ZY * M.YZ + ZZ * M.ZZ};
    }

    // -- Members

    T& XX = P::operator[](0);
    T& XY = P::operator[](1);
    T& XZ = P::operator[](2);
    T& YX = P::operator[](3);
    T& YY = P::operator[](4);
    T& YZ = P::operator[](5);
    T& ZX = P::operator[](6);
    T& ZY = P::operator[](7);
    T& ZZ = P::operator[](8);

    // -- Methods

    static MatrixXYZ<T> identity() { return {1, 0, 0, 0, 1, 0, 0, 0, 1}; }

    MatrixXYZ<T> inverse() const {
        auto det = XX * (YY * ZZ - YZ * ZY) - XY * (YX * ZZ - YZ * ZX) + XZ * (YX * ZY - YY * ZX);
        ASSERT_MSG(det != 0, "MatrixXYZ: singular matrix");

        return {(YY * ZZ - YZ * ZY) / det, (XZ * ZY - XY * ZZ) / det, (XY * YZ - XZ * YY) / det,
                (YZ * ZX - YX * ZZ) / det, (XX * ZZ - XZ * ZX) / det, (XZ * YX - XX * YZ) / det,
                (YX * ZY - YY * ZX) / det, (XY * ZX - XX * ZY) / det, (XX * YY - XY * YX) / det};
    }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const MatrixXYZ& m) {
        return out << "{{" << m.XX << ", " << m.XY << ", " << m.XZ << "}, {" << m.YX << ", " << m.YY << ", " << m.YZ
                   << "}, {" << m.ZX << ", " << m.ZY << ", " << m.ZZ << "}}";
    }
};


}  // namespace eckit::geo::types
