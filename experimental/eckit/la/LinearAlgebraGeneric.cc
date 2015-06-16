/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

//-----------------------------------------------------------------------------

#include "eckit/la/LinearAlgebraGeneric.h"
#include "eckit/la/Matrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());
    Scalar r;
    for (size_t i = 0; i < x.size(); ++i) r += x[i] * y[i];
    return r;
}

//-----------------------------------------------------------------------------

Vector LinearAlgebraGeneric::gemv(const Matrix& A, const Vector& x) const {
    ASSERT(x.size() == A.cols());
    Vector y(A.rows());
    for (size_t r = 0; r < A.rows(); ++r) {
        y[r] = 0.;
        for (size_t c = 0; c < A.cols(); ++c) y[r] += A(r, c) * x[c];
    }
    return y;
}

//-----------------------------------------------------------------------------

Matrix LinearAlgebraGeneric::gemm(const Matrix& A, const Matrix& B) const {
    ASSERT(A.cols() == B.rows());
    Matrix C(A.rows(), B.cols());
    for (size_t c = 0; c < B.cols(); ++c)
        for (size_t r = 0; r < A.rows(); ++r)
            for (size_t k = 0; k < A.cols(); ++k) C(r, c) += A(r, k) * B(k, c);
    return C;
}

//-----------------------------------------------------------------------------

} // namespace la
} // namespace eckit
