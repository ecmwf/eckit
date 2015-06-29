/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#ifdef ECKIT_HAVE_MKL

#include "mkl.h"
#include "mkl_cblas.h"

//-----------------------------------------------------------------------------

#include "experimental/eckit/la/LinearAlgebraFactory.h"
#include "experimental/eckit/la/LinearAlgebraMKL.h"
#include "experimental/eckit/la/Matrix.h"
#include "experimental/eckit/la/SparseMatrix.h"
#include "experimental/eckit/la/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

LinearAlgebraMKL::LinearAlgebraMKL() {
    LinearAlgebraFactory::regist("mkl", this);
}

Scalar LinearAlgebraMKL::dot(const Vector& x, const Vector& y) const {
    ASSERT( x.size() == y.size() );
    return cblas_ddot( x.size(), x.data(), 1, y.data(), 1 );
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    cblas_dgemv( CblasColMajor, CblasNoTrans,
                 A.rows(), A.cols(), 1.0, A.data(), A.rows(), x.data(), 1,
                 0.0, y.data(), 1 );
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    cblas_dgemm( CblasColMajor, CblasNoTrans, CblasNoTrans,
                 A.rows(), B.cols(), A.cols(), 1.0, A.data(), A.rows(),
                 B.data(), B.rows(), 0.0, C.data(), A.rows());
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    MKL_INT m = A.rows();
    MKL_INT k = A.cols();
    double alpha = 1.;
    double beta = 0.;
    mkl_dcsrmv ("N", &m, &k, &alpha, "G__C", A.data(), A.inner(),
                A.outer(), A.outer()+1, x.data(), &beta, y.data());
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    MKL_INT m = A.rows();
    MKL_INT n = C.cols();
    MKL_INT k = A.cols();
    double alpha = 1.;
    double beta = 0.;
    // FIXME: with 0-based indexing, MKL assumes row-major ordering for B and C
    // We need to use 1-based indexing i.e. offset outer and inner indices by 1
    std::vector<MKL_INT> outer(m+1, 1);
    for (size_t i = 0; i < A.rows()+1; ++i)
        outer[i] += A.outer()[i];
    std::vector<MKL_INT> inner(A.nonZeros(), 1);
    for (size_t i = 0; i < A.nonZeros(); ++i)
        inner[i] += A.inner()[i];
    mkl_dcsrmm( "N", &m, &n, &k,
                &alpha, "G__F",
                A.data(), inner.data(), outer.data(), outer.data()+1,
                B.data(), &k, &beta, C.data(), &k);
}

//-----------------------------------------------------------------------------

static LinearAlgebraMKL linearAlgebraMKL;

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif  // ECKIT_HAVE_MKL
