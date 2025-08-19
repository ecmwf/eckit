/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/**
 * @file
 * @author Willem Deconinck
 * @date   Sept 2014
 *
 * Do not include this file directly. Rather include "eckit/maths/Matrix.h" instead.
 *
 * This file implements classes
 *  - RowVector
 *  - ColVector
 *  - Matrix
 *
 * These classes follow Eigen's naming of functions, and could be used
 * as a drop-in replacement, depending on compilation choices
 *
 * The reasoning is that some compilers (e.g. CRAY) have problems compiling
 * Eigen.
 *
 * Where possible, BLAS and LAPACK implementations are used, in case
 * BLAS or LAPACK have been found.
 */

#ifndef eckit_maths_Matrix_h
#error Include "eckit/maths/Matrix.h" instead of "eckit/maths/MatrixLapack.h"
#endif

#ifndef eckit_maths_MatrixLapack_h
#define eckit_maths_MatrixLapack_h

#include <cmath>
#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/Lapack.h"

#ifdef minor
#undef minor
#endif

namespace eckit {

namespace maths {

namespace detail {
namespace ColMajor_4x4 {
template <typename T>
struct remove_const {
    using type = T;
};
template <typename T>
struct remove_const<T const> {
    using type = T;
};

template <typename Scalar>
inline void invert(Scalar m[16], typename remove_const<Scalar>::type inv[16]);
template <typename Scalar>
inline Scalar det(Scalar m[16]);
}  // namespace ColMajor_4x4
}  // namespace detail

template <typename Scalar, typename Index = std::ptrdiff_t>
class Matrix : private eckit::NonCopyable {

protected:

    Scalar* data_{nullptr};
    Index nr_{0};
    Index nc_{0};
    bool is_proxy_{false};

public:

    using MapType      = Matrix;
    using ConstMapType = MapType;

    Matrix() = default;

    // Constructor that allocates matrix with sizes
    template <typename T0, typename T1>
    Matrix(const T0& nr, const T1& nc) {
        resize(nr, nc);
    }

    template <typename T0, typename T1>
    Matrix(Scalar* data, const T0& nr, const T1& nc) : data_{data}, nr_{nr}, nc_{nc}, is_proxy_{true} {}

    // This constructor allows you to construct Matrix from Eigen expressions
    Matrix(const Matrix& other) {
        resize(other.nr_, other.nc_);
        std::memcpy(data_, other.data(), sizeof(Scalar) * nr_ * nc_);
    }

    // Constructor that allocates matrix and infers sizes from initializer list
    Matrix(const std::initializer_list<std::initializer_list<Scalar> >& list) {
        Index nr = list.size();
        if (nr > 0) {
            Index nc = list.begin()->size();
            ASSERT(nc > 0);
            resize(nr, nc);
            Index i{0};
            for (const auto& row : list) {
                ASSERT(row.size() == nc);
                Index j{0};
                for (const auto& elem : row) {
                    at(i, j) = elem;
                    ++j;
                }
                ++i;
            }
        }
    }

    ~Matrix() {
        if (!is_proxy_) {
            delete[] data_;
        }
    }

    Matrix& noalias() { return *this; }

    void resize(Index nr, Index nc) {
        if (is_proxy_) {
            if (nr != nr_ || nc != nc_) {
                throw eckit::Exception("Illegal call: Trying to resize a proxy matrix", Here());
            }
        }
        else {
            if (nr != nr_ || nc != nc_) {
                if (data_) {
                    delete[] data_;
                }
                data_ = new Scalar[nr * nc];
                nr_   = nr;
                nc_   = nc;
            }
        }
    }

    Matrix& operator=(const Matrix& other) {
        resize(other.nr_, other.nc_);
        std::memcpy(data_, other.data(), sizeof(Scalar) * nr_ * nc_);
        return *this;
    }

    Scalar* data() { return data_; }

    const Scalar* data() const { return data_; }

    Index size() const { return nr_ * nc_; }

    Index rows() const { return nr_; }

    Index cols() const { return nc_; }

    template <typename T>
    Scalar& operator[](const T& i) {
        return data_[i];
    }

    template <typename T>
    const Scalar& operator[](const T& i) const {
        return data_[i];
    }

    template <typename T>
    Scalar& operator()(const T& i) {
        return data_[i];
    }

    template <typename T>
    const Scalar& operator()(const T& i) const {
        return data_[i];
    }

    template <typename T0, typename T1>
    Scalar& operator()(const T0& i, const T1& j) {
        return at(i, j);
    }

    template <typename T0, typename T1>
    const Scalar& operator()(const T0& i, const T1& j) const {
        return at(i, j);
    }

    template <typename T>
    Matrix row(const T& i) const {
        Matrix r(1, nc_);
        for (Index j = 0; j < nc_; ++j)
            r(j) = at(i, j);
        return r;
    }

    template <typename T>
    Matrix col(const T& j) const {
        Matrix c(nc_, 1);
        for (Index i = 0; i < nc_; ++i)
            c(i) = at(i, j);
        return c;
    }

    Matrix inverse() const {
        Matrix inv(nr_, nc_);
        switch (nr_) {
            case 1: {
                inv(0, 0) = 1. / at(0, 0);
                break;
            }
            case 2: {
                Scalar deti = 1. / (at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0));
                inv(0, 0)   = at(1, 1) * deti;
                inv(0, 1)   = -at(0, 1) * deti;
                inv(1, 0)   = -at(1, 0) * deti;
                inv(1, 1)   = at(0, 0) * deti;
                break;
            }
            case 3: {
                Scalar deti = 1. / (at(0, 0) * at(1, 1) * at(2, 2) + at(1, 0) * at(2, 1) * at(0, 2) +
                                    at(2, 0) * at(0, 1) * at(1, 2) - at(0, 0) * at(2, 1) * at(1, 2) -
                                    at(2, 0) * at(1, 1) * at(0, 2) - at(1, 0) * at(0, 1) * at(2, 2));

                inv(0, 0) = at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1);
                inv(0, 1) = at(0, 2) * at(2, 1) - at(0, 1) * at(2, 2);
                inv(0, 2) = at(0, 1) * at(1, 2) - at(0, 2) * at(1, 1);
                inv(1, 0) = at(1, 2) * at(2, 0) - at(1, 0) * at(2, 2);
                inv(1, 1) = at(0, 0) * at(2, 2) - at(0, 2) * at(2, 0);
                inv(1, 2) = at(0, 2) * at(1, 0) - at(0, 0) * at(1, 2);
                inv(2, 0) = at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0);
                inv(2, 1) = at(0, 1) * at(2, 0) - at(0, 0) * at(2, 1);
                inv(2, 2) = at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);

                for (Index i = 0; i < 9; ++i)
                    inv.data()[i] *= deti;
                break;
            }
            case 4: {
                detail::ColMajor_4x4::invert(data(), inv.data());
                break;
            }
            default: {  // invert with LU-factorization
#if eckit_HAVE_LAPACK
                inv     = *this;
                int M   = nr_;
                int N   = nc_;
                int lda = nr_;
                int info;
                int* ipiv = new int[std::min(M, N)];
                lapack::getrf(&M, &N, inv.data(), &lda, ipiv, &info);
                if (info == 0) {
                    int lwork    = M * 4;
                    double* work = new double[lwork];
                    lapack::getri(&M, inv.data(), &lda, ipiv, work, &lwork, &info);
                    delete[] work;
                }
                delete[] ipiv;
                if (info > 0) {
                    std::stringstream stream;
                    stream << "U(" << info << "," << info << ")=0 in matrix inversion";
                    throw eckit::Exception(stream.str(), Here());
                }
                else if (info < 0) {
                    std::stringstream stream;
                    stream << "Wrong " << -info << "-th argument in matrix inversion";
                    throw eckit::Exception(stream.str(), Here());
                }
#else
                // This algorithm inverts a matrix based on the Gauss Jordan method.
                int n = nr_;
                Scalar det, pivot, factor;
                Matrix work(n, n);
                det = 1;

                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        inv(i, j)  = 0;
                        work(i, j) = at(i, j);
                    }
                    inv(i, i) = 1.;
                }
                // The current pivot row is jpass.
                // For each pass, first find the maximum element in the pivot column.
                for (int jpass = 0; jpass < n; jpass++) {
                    int imx = jpass;
                    for (int jrow = jpass; jrow < n; jrow++) {
                        if (std::abs(work(jrow, jpass)) > std::abs(work(imx, jpass)))
                            imx = jrow;
                    }
                    // Interchange the elements of row jpass and row imx in both A and AInverse.
                    if (imx != jpass) {
                        for (int jcol = 0; jcol < n; jcol++) {
                            Scalar temp      = inv(jpass, jcol);
                            inv(jpass, jcol) = inv(imx, jcol);
                            inv(imx, jcol)   = temp;

                            if (jcol >= jpass) {
                                temp              = work(jpass, jcol);
                                work(jpass, jcol) = work(imx, jcol);
                                work(imx, jcol)   = temp;
                            }
                        }
                    }

                    // The current pivot is now A[jpass][jpass].
                    // The determinant is the product of the pivot elements.
                    pivot = work(jpass, jpass);
                    det *= pivot;
                    if (det == 0) {
                        throw eckit::SeriousBug("Cannot invert zero determinant matrix", Here());
                    }

                    for (int jcol = 0; jcol < n; jcol++) {
                        // Normalize the pivot row by dividing by the pivot element.
                        inv(jpass, jcol) = inv(jpass, jcol) / pivot;
                        if (jcol >= jpass)
                            work(jpass, jcol) = work(jpass, jcol) / pivot;
                    }

                    for (int jrow = 0; jrow < n; jrow++)
                    // Add a multiple of the pivot row to each row.	The multiple factor
                    // is chosen so that the element of A on the pivot column is 0.
                    {
                        if (jrow != jpass)
                            factor = work(jrow, jpass);
                        for (int jcol = 0; jcol < n; jcol++) {
                            if (jrow != jpass) {
                                inv(jrow, jcol) -= factor * inv(jpass, jcol);
                                work(jrow, jcol) -= factor * work(jpass, jcol);
                            }
                        }
                    }
                }
#endif
            }
        }
        return inv;
    }

    Matrix transpose() const {
        Matrix transposed(nc_, nr_);
        for (Index i = 0; i < nr_; ++i)
            for (Index j = 0; j < nc_; ++j)
                transposed(j, i) = at(i, j);
        return transposed;
    }

    Scalar determinant() const {
        switch (nr_) {
            case 1: {
                return at(0, 0);
            }
            case 2: {
                return (at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0));
            }
            case 3: {
                return (at(0, 0) * at(1, 1) * at(2, 2) + at(1, 0) * at(2, 1) * at(0, 2) +
                        at(2, 0) * at(0, 1) * at(1, 2) - at(0, 0) * at(2, 1) * at(1, 2) -
                        at(2, 0) * at(1, 1) * at(0, 2) - at(1, 0) * at(0, 1) * at(2, 2));
            }
            case 4: {
                return detail::ColMajor_4x4::det(data());
            }
            default: {
                Scalar d = 0;  // value of the determinant
                // this is a matrix of 5x5 or larger
                for (int c = 0; c < cols(); c++) {
                    Matrix M = minor(0, c);
                    d += (2 * ((c + 1) % 2) - 1) * at(0, c) * M.determinant();
                }
                return d;
            }
        }
    }

    template <typename S, typename I>
    friend std::ostream& operator<<(std::ostream& os, const Matrix<S, I>& v);

#define UNARY_OPERATOR_Scalar(OP)              \
    Matrix& operator OP(const Scalar & scal) { \
        for (Index i = 0; i < size(); ++i)     \
            data_[i] OP scal;                  \
        return *this;                          \
    }
    UNARY_OPERATOR_Scalar(+=) UNARY_OPERATOR_Scalar(-=) UNARY_OPERATOR_Scalar(*=) UNARY_OPERATOR_Scalar(/=)
#undef UNARY_OPERATOR_Scalar


        // Matrix Matrix multiply
        Matrix dot(const Matrix& m) const {
        // Very naive implementation. We should use BLAS later
        Matrix result(rows(), m.cols());
        for (Index i = 0; i < rows(); ++i) {
            for (Index j = 0; j < m.cols(); ++j) {
                result(i, j) = 0.;
                for (Index k = 0; k < cols(); ++k)
                    result(i, j) += at(i, k) * m(k, j);
            }
        }
        return result;
    }

    Matrix cwiseProduct(const Matrix& m) const {
        Matrix result(*this);
        for (Index i = 0; i < size(); ++i)
            result.data_[i] *= m.data_[i];
        return result;
    }

    Matrix operator+(const Matrix& m) const {
        Matrix result(*this);
        result += m;
        return result;
    }

    Matrix operator-(const Matrix& m) const {
        Matrix result(*this);
        result -= m;
        return result;
    }

    // Matrix Matrix multiply
    Matrix operator*(const Matrix& m) const { return dot(m); }

    Matrix& operator+=(const Matrix& m) {
        for (Index i = 0; i < size(); ++i)
            data_[i] += m.data_[i];
        return *this;
    }


    Matrix& operator-=(const Matrix& m) {
        for (Index i = 0; i < size(); ++i)
            data_[i] -= m.data_[i];
        return *this;
    }

    bool operator==(const Matrix& m) const {
        if (rows() != m.rows() || cols() != m.cols()) {
            return false;
        }
        for (Index i = 0; i < size(); ++i) {
            if (data_[i] != m.data_[i]) {
                return false;
            }
        }
        return true;
    }

    // Matrix Scalar multiply
    Matrix operator*(const Scalar& s) const {
        Matrix m(*this);
        m *= s;
        return m;
    }


private:

    Scalar& at(const Index i, const Index j) { return data_[i + nr_ * j]; }

    const Scalar& at(const Index i, const Index j) const {
        if (!data_)
            throw eckit::Exception("data_ is null", Here());
        return data_[i + nr_ * j];
    }

    template <typename T0, typename T1>
    Matrix minor(const T0& row, const T1& col) const {
        Matrix m(rows() - 1, cols() - 1);

        Index mr(0);
        for (Index r = 0; r < rows(); ++mr) {
            if (r == row)
                ++r;
            if (r == rows())
                break;
            Index mc(0);
            for (Index c = 0; c < (cols()); ++mc) {
                if (c == col)
                    ++c;
                if (c == cols())
                    break;
                m(mr, mc) = at(r, c);
                ++c;
            }
            ++r;
        }
        return m;
    }
};

template <typename Scalar, typename Index>
class ColVector;

template <typename Scalar, typename Index = std::ptrdiff_t>
class RowVector : public Matrix<Scalar, Index> {
    using Base = Matrix<Scalar, Index>;

public:

    using MapType      = RowVector;
    using ConstMapType = MapType;

public:

    RowVector() : Base() {}

    // Constructor that allocates matrix with sizes
    template <typename T0>
    RowVector(const T0& nc) : Base(1, nc) {}

    template <typename T0>
    RowVector(Scalar* data, const T0& nc) : Base(data, 1, nc) {}

    // This constructor allows you to construct Matrix from Eigen expressions
    RowVector(const Base& other) : Base(other) {}

    void resize(Index nc) { Base::resize(1, nc); }

    RowVector& operator=(const Base& other) {
        resize(other.cols());
        std::memcpy(this->data(), other.data(), sizeof(Scalar) * this->cols());
        return *this;
    }

    // RowVector ColVector multiply
    Scalar operator*(const ColVector<Scalar, Index>& c) const {
        Scalar s(0);
        for (Index j = 0; j < Base::nc_; ++j)
            s += Base::data_[j] * c[j];
        return s;
    }

    // RowVector Matrix multiply
    Base operator*(const Base& m) const { return Base::operator*(m); }
};

template <typename Scalar, typename Index = std::ptrdiff_t>
class ColVector : public Matrix<Scalar, Index> {
    using Base = Matrix<Scalar, Index>;

public:

    using MapType      = ColVector;
    using ConstMapType = MapType;

public:

    ColVector() : Base() {}

    // Constructor that allocates matrix with sizes
    template <typename T0>
    ColVector(const T0& nr) : Base(nr, 1) {}

    template <typename T0>
    ColVector(Scalar* data, const T0& nr) : Base(data, nr, 1) {}

    // This constructor allows you to construct Matrix from Eigen expressions
    ColVector(const Base& other) : Base(other) {}

    void resize(Index nr) { Base::resize(nr, 1); }

    ColVector& operator=(const Base& other) {
        resize(other.rows());
        std::memcpy(this->data(), other.data(), sizeof(Scalar) * this->rows());
        return *this;
    }

    // ColVector RowVector multiply
    Matrix<Scalar, Index> operator*(const RowVector<Scalar, Index>& r) const {
        Matrix<Scalar, Index> m(Base::nc_, r.nr_);
        for (Index i = 0; i < m.nr_; ++i) {
            for (Index j = 0; j < m.nc_; ++j) {
                m(i, j) = Base::data_[i] * r[j];
            }
        }
        return m;
    }
};


template <typename Scalar, typename Index>
std::ostream& operator<<(std::ostream& os, const Matrix<Scalar, Index>& m) {
    for (Index i = 0; i < m.nr_; ++i) {
        if (i > 0)
            os << "\n";
        for (Index j = 0; j < m.nc_; ++j) {
            os << m(i, j) << " ";
        }
    }
    return os;
}

namespace detail {

namespace ColMajor_4x4 {

template <typename Scalar>
inline void invert(Scalar m[16], typename remove_const<Scalar>::type inv[16]) {

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
             m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
             m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
             m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] -
              m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] -
             m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
             m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
             m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
              m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
             m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
             m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
              m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
              m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
             m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
              m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    typename remove_const<Scalar>::type det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) {
        Matrix<Scalar> mat(m, 4, 4);
        std::stringstream msg;
        msg << "Trying to invert 4x4 matrix with zero determinant.\nMatrix = \n" << mat;
        throw eckit::Exception(msg.str(), Here());
    }

    det = 1. / det;

    for (int i = 0; i < 16; i++)
        inv[i] *= det;
}

template <typename Scalar>
Scalar det(Scalar m[16]) {
    Scalar inv[4] = {
        m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] -
            m[13] * m[7] * m[10],
        -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] -
            m[12] * m[6] * m[11] + m[12] * m[7] * m[10],
        m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] -
            m[12] * m[7] * m[9],
        -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] +
            m[12] * m[6] * m[9],
    };
    return m[0] * inv[0] + m[1] * inv[1] + m[2] * inv[2] + m[3] * inv[3];
}

}  // namespace ColMajor_4x4

namespace RowMajor_4x4 {

// minor
template <typename Scalar>
inline Scalar mnr(Scalar m[16], int r0, int r1, int r2, int c0, int c1, int c2) {
    return m[4 * r0 + c0] * (m[4 * r1 + c1] * m[4 * r2 + c2] - m[4 * r2 + c1] * m[4 * r1 + c2]) -
           m[4 * r0 + c1] * (m[4 * r1 + c0] * m[4 * r2 + c2] - m[4 * r2 + c0] * m[4 * r1 + c2]) +
           m[4 * r0 + c2] * (m[4 * r1 + c0] * m[4 * r2 + c1] - m[4 * r2 + c0] * m[4 * r1 + c1]);
}

template <typename Scalar>
inline void adjoint(Scalar m[16], Scalar adj[16]) {
    adj[0]  = mnr(m, 1, 2, 3, 1, 2, 3);
    adj[1]  = -mnr(m, 0, 2, 3, 1, 2, 3);
    adj[2]  = mnr(m, 0, 1, 3, 1, 2, 3);
    adj[3]  = -mnr(m, 0, 1, 2, 1, 2, 3);
    adj[4]  = -mnr(m, 1, 2, 3, 0, 2, 3);
    adj[5]  = mnr(m, 0, 2, 3, 0, 2, 3);
    adj[6]  = -mnr(m, 0, 1, 3, 0, 2, 3);
    adj[7]  = mnr(m, 0, 1, 2, 0, 2, 3);
    adj[8]  = mnr(m, 1, 2, 3, 0, 1, 3);
    adj[9]  = -mnr(m, 0, 2, 3, 0, 1, 3);
    adj[10] = mnr(m, 0, 1, 3, 0, 1, 3);
    adj[11] = -mnr(m, 0, 1, 2, 0, 1, 3);
    adj[12] = -mnr(m, 1, 2, 3, 0, 1, 2);
    adj[13] = mnr(m, 0, 2, 3, 0, 1, 2);
    adj[14] = -mnr(m, 0, 1, 3, 0, 1, 2);
    adj[15] = mnr(m, 0, 1, 2, 0, 1, 2);
}

template <typename Scalar>
inline Scalar det(Scalar m[16]) {
    return m[0] * mnr(m, 1, 2, 3, 1, 2, 3) - m[1] * mnr(m, 1, 2, 3, 0, 2, 3) + m[2] * mnr(m, 1, 2, 3, 0, 1, 3) -
           m[3] * mnr(m, 1, 2, 3, 0, 1, 2);
}

template <typename Scalar>
inline void invert(Scalar m[16], Scalar inv[16]) {
    adjoint(m, inv);
    Scalar inv_det = 1.0 / det(m);
    for (int i = 0; i < 16; ++i)
        inv[i] = inv[i] * inv_det;
}

}  // namespace RowMajor_4x4


}  // namespace detail

}  // namespace maths

}  // namespace eckit

#endif
