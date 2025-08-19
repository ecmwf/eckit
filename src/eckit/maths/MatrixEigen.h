/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_maths_Matrix_h
#error Include "eckit/maths/Matrix.h" instead of "eckit/maths/MatrixEigen.h"
#endif

#ifndef eckit_maths_MatrixEigen_h
#define eckit_maths_MatrixEigen_h

#include "eckit/maths/Eigen.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace maths {

//========================================================================================

/// @brief Matrix class, with internal Column-Major storage ordering
///
/// This class is inherited from Eigen's matrix class
/// Eigen::ColMajor is also Eigen's default (recommended) storage ordering.
/// Furthermore it is directly compatible with Fortran matrix manipulation routines
template <typename scalar, typename index = std::ptrdiff_t>
class Matrix : public Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> {
    using Base = Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

public:

    // Default constructor
    Matrix(void) : Base() {}

    // Constructor that allocates matrix with sizes
    template <typename T0, typename T1>
    Matrix(const T0& x, const T1& y) : Base(x, y) {}

    // Constructor that allocates matrix and infers sizes from initializer list
    Matrix(const std::initializer_list<std::initializer_list<scalar>>& list) :
#if EIGEN_VERSION_AT_LEAST(3, 4, 0)
        Base(list) {
    }
#else
        Base() {  // Old Eigen versions don't have initializer list initialization :(
        size_t nr = list.size();
        if (nr > 0) {
            index nc = list.begin()->size();
            ASSERT(nc > 0);
            this->resize(nr, nc);
            size_t i{0};
            for (const auto& row : list) {
                ASSERT(row.size() == nc);
                size_t j{0};
                for (const auto& elem : row) {
                    (*this)(i, j) = elem;
                    ++j;
                }
                ++i;
            }
        }
    }
#endif

    // This constructor allows you to construct Matrix from Eigen expressions
    template <typename OtherDerived>
    Matrix(const Eigen::MatrixBase<OtherDerived>& other) : Base(other) {}

    // This method allows you to assign Eigen expressions to Matrix
    template <typename OtherDerived>
    Matrix& operator=(const Eigen::MatrixBase<OtherDerived>& other) {
        this->Base::operator=(other);
        return *this;
    }
};

//========================================================================================

/// @brief Row-Vector class
///
/// This class is inherited from Eigen's matrix class
template <typename scalar, typename index = std::ptrdiff_t>
class RowVector : public Eigen::Matrix<scalar, 1, Eigen::Dynamic> {
    using Base = Eigen::Matrix<scalar, 1, Eigen::Dynamic>;

public:

    // Default constructor
    RowVector(void) : Base() {}

    // Constructor that allocates matrix with sizes
    template <typename T>
    RowVector(const T& x) : Base(x) {}

    // This constructor allows you to construct Matrix from Eigen expressions
    template <typename OtherDerived>
    RowVector(const Eigen::MatrixBase<OtherDerived>& other) : Base(other) {}

    // This method allows you to assign Eigen expressions to Matrix
    template <typename OtherDerived>
    RowVector& operator=(const Eigen::MatrixBase<OtherDerived>& other) {
        this->Base::operator=(other);
        return *this;
    }
};

//========================================================================================

/// @brief Column-Vector class
///
/// This class is inherited from Eigen's matrix class
template <typename scalar, typename index = std::ptrdiff_t>
class ColVector : public Eigen::Matrix<scalar, Eigen::Dynamic, 1> {
    using Base = Eigen::Matrix<scalar, Eigen::Dynamic, 1>;

public:

    // Default constructor
    ColVector(void) : Base() {}

    // Constructor that allocates matrix with sizes
    template <typename T>
    ColVector(const T& x) : Base(x) {}

    // This constructor allows you to construct Matrix from Eigen expressions
    template <typename OtherDerived>
    ColVector(const Eigen::MatrixBase<OtherDerived>& other) : Base(other) {}

    // This method allows you to assign Eigen expressions to Matrix
    template <typename OtherDerived>
    ColVector& operator=(const Eigen::MatrixBase<OtherDerived>& other) {
        this->Base::operator=(other);
        return *this;
    }
};

//========================================================================================

}  // namespace maths

}  // namespace eckit

#endif
