/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/dense/LinearAlgebraMKL.h"

#include "mkl.h"
#include "mkl_cblas.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {
namespace dense {


static const LinearAlgebraMKL __la("mkl");


void LinearAlgebraMKL::print(std::ostream& out) const {
    out << "LinearAlgebraMKL[]";
}


Scalar LinearAlgebraMKL::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    // double cblas_ddot (const MKL_INT n, const double *x, const MKL_INT incx, const double *y, const MKL_INT incy);
    return cblas_ddot(x.size(), x.data(), 1, y.data(), 1);
}


void LinearAlgebraMKL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // void cblas_dgemv (const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE trans,
    //                   const MKL_INT m, const MKL_INT n,
    //                   const double alpha, const double a, const MKL_INT lda, const double *x, const MKL_INT incx,
    //                   const double beta, double *y, const MKL_INT incy);
    cblas_dgemv(CblasColMajor, CblasNoTrans, A.rows(), A.cols(), 1.0, A.data(), A.rows(), x.data(), 1, 0.0, y.data(),
                1);
}


void LinearAlgebraMKL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // void cblas_dgemm (const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE transa, const CBLAS_TRANSPOSE transb,
    //                   const MKL_INT m, const MKL_INT n, const MKL_INT k,
    //                   const double alpha, const double a, const MKL_INT lda, const double *b, const MKL_INT ldb,
    //                   const double beta, double *c, const MKL_INT ldc);
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, A.rows(), B.cols(), A.cols(), 1.0, A.data(), A.rows(),
                B.data(), B.rows(), 0.0, C.data(), A.rows());
}


}  // namespace dense
}  // namespace linalg
}  // namespace eckit
