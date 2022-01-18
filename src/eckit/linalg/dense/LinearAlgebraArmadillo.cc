/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/dense/LinearAlgebraArmadillo.h"

#include <armadillo>

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {
namespace dense {


static const LinearAlgebraArmadillo __la("armadillo");


using vec_t = arma::vec;
using mat_t = arma::mat;


void LinearAlgebraArmadillo::print(std::ostream& out) const {
    out << "LinearAlgebraArmadillo[]";
}


Scalar LinearAlgebraArmadillo::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    // Armadillo requires non-const pointers to the data for views without copy
    vec_t xi(const_cast<Scalar*>(x.data()), x.size(), /* copy_aux_mem= */ false);
    vec_t yi(const_cast<Scalar*>(y.data()), y.size(), /* copy_aux_mem= */ false);

    return arma::dot(xi, yi);
}


void LinearAlgebraArmadillo::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // Armadillo requires non-const pointers to the data for views without copy
    mat_t Ai(const_cast<Scalar*>(A.data()), A.rows(), A.cols(), /* copy_aux_mem= */ false);
    vec_t xi(const_cast<Scalar*>(x.data()), x.size(), /* copy_aux_mem= */ false);
    vec_t yi(y.data(), y.size(), /* copy_aux_mem= */ false);

    yi = Ai * xi;
}


void LinearAlgebraArmadillo::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // Armadillo requires non-const pointers to the data for views without copy
    mat_t Ai(const_cast<Scalar*>(A.data()), A.rows(), A.cols(), /* copy_aux_mem= */ false);
    mat_t Bi(const_cast<Scalar*>(B.data()), B.rows(), B.cols(), /* copy_aux_mem= */ false);
    mat_t Ci(C.data(), C.rows(), C.cols(), /* copy_aux_mem= */ false);

    Ci = Ai * Bi;
}


}  // namespace dense
}  // namespace linalg
}  // namespace eckit
