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

#include "eckit/eckit_config.h"

#ifdef ECKIT_HAVE_EIGEN

#include <Eigen/Core>

#include "eckit/la/LinearAlgebraEigen.h"
#include "eckit/la/LinearAlgebraFactory.h"
#include "eckit/la/Matrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

LinearAlgebraEigen::LinearAlgebraEigen() {
    LinearAlgebraFactory::regist("eigen", this);
}

//-----------------------------------------------------------------------------

Scalar LinearAlgebraEigen::dot(const Vector& x, const Vector& y) const {
    ASSERT( x.size() == y.size() );
    // Eigen requires non-const pointers to the data
    Eigen::VectorXd::MapType xi = Eigen::VectorXd::Map( const_cast<Scalar*>(x.data()), x.size() );
    Eigen::VectorXd::MapType yi = Eigen::VectorXd::Map( const_cast<Scalar*>(y.data()), y.size() );
    return xi.dot(yi);
}

//-----------------------------------------------------------------------------

void LinearAlgebraEigen::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // Eigen requires non-const pointers to the data
    Eigen::MatrixXd::MapType Ai = Eigen::MatrixXd::Map( const_cast<Scalar*>(A.data()), A.rows(), A.cols() );
    Eigen::VectorXd::MapType xi = Eigen::VectorXd::Map( const_cast<Scalar*>(x.data()), x.size() );
    Eigen::VectorXd::MapType yi = Eigen::VectorXd::Map( y.data(), y.size() );
    yi = Ai*xi;
}

//-----------------------------------------------------------------------------

void LinearAlgebraEigen::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // Eigen requires non-const pointers to the data
    Eigen::MatrixXd::MapType Ai = Eigen::MatrixXd::Map( const_cast<Scalar*>(A.data()), A.rows(), A.cols() );
    Eigen::MatrixXd::MapType Bi = Eigen::MatrixXd::Map( const_cast<Scalar*>(B.data()), B.rows(), B.cols() );
    Eigen::MatrixXd::MapType Ci = Eigen::MatrixXd::Map( C.data(), C.rows(), C.cols() );
    Ci = Ai*Bi;
}

//-----------------------------------------------------------------------------

void LinearAlgebraEigen::spmv(const SparseMatrix&, const Vector&, Vector&) const {
    NOTIMP;
}

//-----------------------------------------------------------------------------

void LinearAlgebraEigen::spmm(const SparseMatrix&, const Matrix&, Matrix&) const {
    NOTIMP;
}

//-----------------------------------------------------------------------------

static LinearAlgebraEigen linearAlgebraEigen;

//-----------------------------------------------------------------------------

} // namespace la
}  // namespace eckit

#endif  // ECKIT_HAVE_EIGEN
