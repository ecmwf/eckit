/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/linalg/LinearAlgebraMKL.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

LinearAlgebraMKL::LinearAlgebraMKL() : LinearAlgebra("mkl") {}

Scalar LinearAlgebraMKL::dot(const Vector& x, const Vector& y) const {
    ASSERT( x.size() == y.size() );
    // double cblas_ddot (const MKL_INT n, const double *x, const MKL_INT incx, const double *y, const MKL_INT incy);
    return cblas_ddot( x.size(), x.data(), 1, y.data(), 1 );
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // void cblas_dgemv (const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE trans,
    //                   const MKL_INT m, const MKL_INT n,
    //                   const double alpha, const double a, const MKL_INT lda, const double *x, const MKL_INT incx,
    //                   const double beta, double *y, const MKL_INT incy);
    cblas_dgemv( CblasColMajor, CblasNoTrans,
                 A.rows(), A.cols(),
                 1.0, A.data(), A.rows(), x.data(), 1,
                 0.0, y.data(), 1 );
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // void cblas_dgemm (const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE transa, const CBLAS_TRANSPOSE transb,
    //                   const MKL_INT m, const MKL_INT n, const MKL_INT k,
    //                   const double alpha, const double a, const MKL_INT lda, const double *b, const MKL_INT ldb,
    //                   const double beta, double *c, const MKL_INT ldc);
    cblas_dgemm( CblasColMajor, CblasNoTrans, CblasNoTrans,
                 A.rows(), B.cols(), A.cols(),
                 1.0, A.data(), A.rows(), B.data(), B.rows(),
                 0.0, C.data(), A.rows());
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
    MKL_INT m = A.rows();
    MKL_INT k = A.cols();
    double alpha = 1.;
    double beta = 0.;
    // void mkl_dcsrmv (char *transa, MKL_INT *m, MKL_INT *k,
    //                  double *alpha, char *matdescra,
    //                  double *val, MKL_INT *indx, MKL_INT *pntrb, MKL_INT *pntre,
    //                  double *x, double *beta, double *y);
    // std::cout << "Calling MKL::spmv()" << std::endl;
    mkl_dcsrmv ("N", &m, &k,
                &alpha, "G__C",
                A.data(), A.inner(), A.outer(), A.outer()+1,
                x.data(), &beta, y.data());
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
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
    // void mkl_dcsrmm (char *transa, MKL_INT *m, MKL_INT *n, MKL_INT *k,
    //                  double *alpha, char *matdescra,
    //                  double *val, MKL_INT *indx, MKL_INT *pntrb, MKL_INT *pntre,
    //                  double *b, MKL_INT *ldb, double *beta, double *c, MKL_INT *ldc);
    mkl_dcsrmm( "N", &m, &n, &k,
                &alpha, "G__F",
                A.data(), inner.data(), outer.data(), outer.data()+1,
                B.data(), &k, &beta, C.data(), &k);
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    LinearAlgebra::getBackend("generic").dsptd(x, A, y, B);
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::print(std::ostream& out) const {
    out << "LinearAlgebraMKL[]";
}

//-----------------------------------------------------------------------------

static LinearAlgebraMKL linearAlgebraMKL;

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif  // ECKIT_HAVE_MKL
