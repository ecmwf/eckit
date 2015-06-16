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
    return cblas_ddot( x.size(), x.data(), 1, y.data(), 1 );
}

//-----------------------------------------------------------------------------

Vector LinearAlgebraMKL::gemv(const Matrix& A, const Vector& x) const {
    Vector rv(A.rows());
    cblas_dgemv( CblasColMajor, CblasNoTrans,
                 A.rows(), A.cols(), 1.0, A.data(), A.rows(), x.data(), 1,
                 0.0, rv.data(), 1 );
    return rv;
}

//-----------------------------------------------------------------------------

Matrix LinearAlgebraMKL::gemm(const Matrix& A, const Matrix& B) const {
    Matrix C(A.rows(), B.cols());
    cblas_dgemm( CblasColMajor, CblasNoTrans, CblasNoTrans,
                 A.rows(), B.cols(), A.cols(), 1.0, A.data(), A.rows(),
                 B.data(), B.rows(),0.0, C.data(), A.rows());
  return C;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif  // ECKIT_HAVE_MKL
