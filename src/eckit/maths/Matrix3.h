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
#include "eckit/geo/PointXYZ.h"


namespace eckit::maths {


template <typename T>
class Matrix3 final : protected std::array<T, 9> {
public:

    // -- Types

    using container_type = std::array<T, 9>;

public:

    // -- Constructors

    Matrix3(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) :
        container_type{xx, xy, xz, yx, yy, yz, zx, zy, zz} {}

    Matrix3(const Matrix3& other) : container_type(other) {}
    Matrix3(Matrix3&& other) : container_type(other) {}

    // -- Destructor

    ~Matrix3() = default;

    // -- Operators

    Matrix3& operator=(const Matrix3<T>& other) {
        container_type::operator=(other);
        return *this;
    }

    Matrix3& operator=(Matrix3<T>&& other) {
        container_type::operator=(other);
        return *this;
    }

    geo::PointXYZ operator*(const geo::PointXYZ& p) const {
        return {XX * p.X + XY * p.Y + XZ * p.Z, YX * p.X + YY * p.Y + YZ * p.Z, ZX * p.X + ZY * p.Y + ZZ * p.Z};
    }

    Matrix3<T> operator*(const Matrix3<T>& M) const {
        return {
            XX * M.XX + XY * M.YX + XZ * M.ZX, XX * M.XY + XY * M.YY + XZ * M.ZY, XX * M.XZ + XY * M.YZ + XZ * M.ZZ,
            YX * M.XX + YY * M.YX + YZ * M.ZX, YX * M.XY + YY * M.YY + YZ * M.ZY, YX * M.XZ + YY * M.YZ + YZ * M.ZZ,
            ZX * M.XX + ZY * M.YX + ZZ * M.ZX, ZX * M.XY + ZY * M.YY + ZZ * M.ZY, ZX * M.XZ + ZY * M.YZ + ZZ * M.ZZ};
    }

    // -- Members

    const T& XX = container_type::operator[](0);
    const T& XY = container_type::operator[](1);
    const T& XZ = container_type::operator[](2);
    const T& YX = container_type::operator[](3);
    const T& YY = container_type::operator[](4);
    const T& YZ = container_type::operator[](5);
    const T& ZX = container_type::operator[](6);
    const T& ZY = container_type::operator[](7);
    const T& ZZ = container_type::operator[](8);

    // -- Methods

    using container_type::begin;
    using container_type::cbegin;
    using container_type::cend;
    using container_type::end;
    using container_type::size;

    static Matrix3<T> identity() { return {1, 0, 0, 0, 1, 0, 0, 0, 1}; }

    Matrix3<T> inverse() const {
        auto det = XX * (YY * ZZ - YZ * ZY) - XY * (YX * ZZ - YZ * ZX) + XZ * (YX * ZY - YY * ZX);
        ASSERT_MSG(det != 0, "Matrix3: singular matrix");

        return {(YY * ZZ - YZ * ZY) / det, (XZ * ZY - XY * ZZ) / det, (XY * YZ - XZ * YY) / det,
                (YZ * ZX - YX * ZZ) / det, (XX * ZZ - XZ * ZX) / det, (XZ * YX - XX * YZ) / det,
                (YX * ZY - YY * ZX) / det, (XY * ZX - XX * ZY) / det, (XX * YY - XY * YX) / det};
    }

    T determinant() const {
        return XX * YY * ZZ - XZ * YY * ZX + XY * YZ * ZX + XZ * YX * ZY - XX * YZ * ZY - XY * YX * ZZ;
    }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Matrix3& m) {
        return out << "{{" << m.XX << ", " << m.XY << ", " << m.XZ << "}, {" << m.YX << ", " << m.YY << ", " << m.YZ
                   << "}, {" << m.ZX << ", " << m.ZY << ", " << m.ZZ << "}}";
    }
};


}  // namespace eckit::maths
