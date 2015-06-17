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

#include "eckit/la/LinearAlgebraMKL.h"
#include "eckit/la/Matrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

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

void LinearAlgebraMKL::spmv(const SparseMatrix&, const Vector&, Vector&) const {
    NOTIMP;
}

//-----------------------------------------------------------------------------

void LinearAlgebraMKL::spmm(const SparseMatrix&, const Matrix&, Matrix&) const {
    NOTIMP;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif  // ECKIT_HAVE_MKL
