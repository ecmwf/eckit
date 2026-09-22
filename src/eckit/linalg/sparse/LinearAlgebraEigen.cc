// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/sparse/LinearAlgebraEigen.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"
#include "eckit/maths/Eigen.h"

namespace eckit::linalg::sparse {

static const LinearAlgebraEigen __la("eigen");

using vec_t = Eigen::VectorXd::MapType;
using mat_t = Eigen::MatrixXd::MapType;
using spm_t = Eigen::Map<Eigen::SparseMatrix<Scalar, Eigen::RowMajor, Index>>;


void LinearAlgebraEigen::print(std::ostream& out) const {
    out << "LinearAlgebraEigen[]";
}


void LinearAlgebraEigen::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);

    // Eigen requires non-const pointers to the data
    spm_t Ai(A.rows(), A.cols(), A.nonZeros(), const_cast<Index*>(A.outer()), const_cast<Index*>(A.inner()),
             const_cast<Scalar*>(A.data()));
    vec_t xi(Eigen::VectorXd::Map(const_cast<Scalar*>(x.data()), x.size()));
    vec_t yi(Eigen::VectorXd::Map(y.data(), y.size()));

    yi = Ai * xi;
}


void LinearAlgebraEigen::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);

    // Eigen requires non-const pointers to the data
    spm_t Ai(A.rows(), A.cols(), A.nonZeros(), const_cast<Index*>(A.outer()), const_cast<Index*>(A.inner()),
             const_cast<Scalar*>(A.data()));
    mat_t Bi(Eigen::MatrixXd::Map(const_cast<Scalar*>(B.data()), B.rows(), B.cols()));
    mat_t Ci(Eigen::MatrixXd::Map(C.data(), C.rows(), C.cols()));

    Ci = Ai * Bi;
}


void LinearAlgebraEigen::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}

}  // namespace eckit::linalg::sparse
