/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/LinearAlgebraLAPACK.h"

#ifdef ECKIT_HAVE_LAPACK

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"

//----------------------------------------------------------------------------------------------------------------------

extern "C" {

double ddot_(const int* n, const double* x, const int* incx, const double* y, const int* incy);

void dgemv_(const char* transa, const int* m, const int* n, const double* alpha, const double* a, const int* lda,
            const double* x, const int* incx, const double* beta, double* y, const int* incy);

void dgemm_(const char* transa, const char* transb, const int* m, const int* n, const int* k, const double* alpha,
            const double* a, const int* lda, const double* b, const int* ldb, const double* beta, double* c,
            const int* ldc);
}

static const char* trans  = "N";
static const int inc      = 1;
static const double alpha = 1.;
static const double beta  = 0.;

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

LinearAlgebraLAPACK::LinearAlgebraLAPACK() : LinearAlgebra("lapack") {}

Scalar LinearAlgebraLAPACK::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    const int n = int(x.size());

    return ddot_(&n, x.data(), &inc, y.data(), &inc);
}

void LinearAlgebraLAPACK::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols() && y.size() == A.rows());

    const int m = int(A.rows());
    const int n = int(A.cols());

    dgemv_(trans, &m, &n, &alpha, A.data(), &m, x.data(), &inc, &beta, y.data(), &inc);
}

void LinearAlgebraLAPACK::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols());

    const int m = int(A.rows());
    const int n = int(B.cols());
    const int k = int(A.cols());

    dgemm_(trans, trans, &m, &n, &k, &alpha, A.data(), &m, B.data(), &k, &beta, C.data(), &m);
}

void LinearAlgebraLAPACK::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    LinearAlgebra::getBackend("generic").spmv(A, x, y);
}

void LinearAlgebraLAPACK::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    LinearAlgebra::getBackend("generic").spmm(A, B, C);
}

void LinearAlgebraLAPACK::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    LinearAlgebra::getBackend("generic").dsptd(x, A, y, B);
}

void LinearAlgebraLAPACK::print(std::ostream& out) const {
    out << "LinearAlgebraLAPACK[]";
}

//----------------------------------------------------------------------------------------------------------------------

static LinearAlgebraLAPACK __la;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit

#endif  // ECKIT_HAVE_LAPACK
