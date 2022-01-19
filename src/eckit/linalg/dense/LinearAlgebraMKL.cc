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

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {
namespace dense {


static const LinearAlgebraMKL __la("mkl");

static const MKL_INT inc           = 1;
static const CBLAS_LAYOUT layout   = CblasColMajor;
static const CBLAS_TRANSPOSE trans = CblasNoTrans;
static const double alpha          = 1.;
static const double beta           = 0.;


void LinearAlgebraMKL::print(std::ostream& out) const {
    out << "LinearAlgebraMKL[]";
}


Scalar LinearAlgebraMKL::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    const auto n   = static_cast<const MKL_INT>(x.size());
    const auto* _x = static_cast<const double*>(x.data());
    const auto* _y = static_cast<const double*>(y.data());

    // double cblas_ddot(const MKL_INT N, const double *X, const MKL_INT incX,
    //                   const double *Y, const MKL_INT incY);

    return cblas_ddot(n, _x, inc, _y, inc);
}


void LinearAlgebraMKL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    const auto m = static_cast<const MKL_INT>(A.rows());
    const auto n = static_cast<const MKL_INT>(A.cols());

    const auto* _A = static_cast<const double*>(A.data());
    const auto* _x = static_cast<const double*>(x.data());
    auto* _y       = static_cast<double*>(y.data());

    // void cblas_dgemv(const CBLAS_LAYOUT Layout,
    //                  const CBLAS_TRANSPOSE TransA, const MKL_INT M, const MKL_INT N,
    //                  const double alpha, const double *A, const MKL_INT lda,
    //                  const double *X, const MKL_INT incX, const double beta,
    //                  double *Y, const MKL_INT incY);

    cblas_dgemv(layout, trans, m, n, alpha, _A, m, _x, inc, beta, _y, inc);
}


void LinearAlgebraMKL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    const auto m = static_cast<const MKL_INT>(A.rows());
    const auto n = static_cast<const MKL_INT>(B.cols());
    const auto k = static_cast<const MKL_INT>(A.cols());

    const auto* _A = static_cast<const double*>(A.data());
    const auto* _B = static_cast<const double*>(B.data());
    auto* _C       = static_cast<double*>(C.data());

    // void cblas_dgemm(const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE TransA,
    //                  const CBLAS_TRANSPOSE TransB, const MKL_INT M, const MKL_INT N,
    //                  const MKL_INT K, const double alpha, const double *A,
    //                  const MKL_INT lda, const double *B, const MKL_INT ldb,
    //                  const double beta, double *C, const MKL_INT ldc);

    cblas_dgemm(layout, trans, trans, m, n, k, alpha, _A, m, _B, k, beta, _C, m);
}


}  // namespace dense
}  // namespace linalg
}  // namespace eckit
