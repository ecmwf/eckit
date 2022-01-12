/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/eckit.h"

#ifdef eckit_HAVE_LAPACK

#include "eckit/linalg/LinearAlgebraLAPACK.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"


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


namespace eckit {
namespace linalg {


namespace {
static const std::string __name{"lapack"};

static const dense::LinearAlgebraLAPACK __lad(__name);
static const deprecated::LinearAlgebraLAPACK __la(__name);
}  // anonymous namespace


namespace dense {


//-----------------------------------------------------------------------------


void LinearAlgebraLAPACK::print(std::ostream& out) const {
    out << "LinearAlgebraLAPACK[]";
}


Scalar LinearAlgebraLAPACK::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    const auto n = int(x.size());

    return ddot_(&n, x.data(), &inc, y.data(), &inc);
}


void LinearAlgebraLAPACK::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    const auto m = int(A.rows());
    const auto n = int(A.cols());

    dgemv_(trans, &m, &n, &alpha, A.data(), &m, x.data(), &inc, &beta, y.data(), &inc);
}


void LinearAlgebraLAPACK::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    const auto m = int(A.rows());
    const auto n = int(B.cols());
    const auto k = int(A.cols());

    dgemm_(trans, trans, &m, &n, &k, &alpha, A.data(), &m, B.data(), &k, &beta, C.data(), &m);
}


//-----------------------------------------------------------------------------


}  // namespace dense
}  // namespace linalg
}  // namespace eckit


#endif  // eckit_HAVE_LAPACK
