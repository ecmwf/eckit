/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/LinearAlgebraOpenMP.h"

#include <ostream>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

LinearAlgebraOpenMP::LinearAlgebraOpenMP() :
    LinearAlgebra("openmp") {}

Scalar LinearAlgebraOpenMP::dot(const Vector& x, const Vector& y) const {
    return LinearAlgebra::getBackend("generic").dot(x, y);
}

void LinearAlgebraOpenMP::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    LinearAlgebra::getBackend("generic").gemv(A, x, y);
}

void LinearAlgebraOpenMP::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    LinearAlgebra::getBackend("generic").gemm(A, B, C);
}

void LinearAlgebraOpenMP::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    LinearAlgebra::getBackend("generic").spmv(A, x, y);
}

void LinearAlgebraOpenMP::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    const auto outer = A.outer();
    const auto inner = A.inner();
    const auto val   = A.data();

    const auto Ni = A.rows();
    const auto Nj = A.cols();
    const auto Nk = B.cols();

    ASSERT(C.rows() == Ni);
    ASSERT(B.rows() == Nj);
    ASSERT(C.cols() == Nk);

#pragma omp parallel for
    for (Size i = 0; i < Ni; ++i) {
        std::vector<Scalar> sum(Nk, 0);  // private (hopefully not reallocated!)

        for (auto c = outer[i]; c < outer[i + 1]; ++c) {
            auto j = static_cast<Size>(inner[c]);
            auto v = val[c];
            for (Size k = 0; k < Nk; ++k) {
                sum[k] += v * B(j, k);
            }
        }

        for (Size k = 0; k < Nk; ++k) {
            C(i, k) = sum[k];
        }
    }
}

void LinearAlgebraOpenMP::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    LinearAlgebra::getBackend("generic").dsptd(x, A, y, B);
}

void LinearAlgebraOpenMP::print(std::ostream& out) const {
    out << "LinearAlgebraOpenMP[]";
}

//----------------------------------------------------------------------------------------------------------------------

static LinearAlgebraOpenMP __la;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
