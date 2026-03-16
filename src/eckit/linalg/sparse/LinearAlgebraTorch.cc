/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraTorch.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/detail/Torch.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit::linalg::sparse {


static const LinearAlgebraTorch LA_TORCH_CPU_1("torch", torch::DeviceType::CPU);
static const LinearAlgebraTorch LA_TORCH_CPU_2("torch-cpu", torch::DeviceType::CPU);
static const LinearAlgebraTorch LA_TORCH_CUDA("torch-cuda", torch::DeviceType::CUDA);
static const LinearAlgebraTorch LA_TORCH_HIP("torch-hip", torch::DeviceType::HIP);
// static const LinearAlgebraTorch LA_TORCH_MPS("torch-mps", torch::DeviceType::MPS);
static const LinearAlgebraTorch LA_TORCH_XPU("torch-xpu", torch::DeviceType::XPU);
static const LinearAlgebraTorch LA_TORCH_XLA("torch-xla", torch::DeviceType::XLA);
static const LinearAlgebraTorch LA_TORCH_META("torch-meta", torch::DeviceType::Meta);


void LinearAlgebraTorch::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    ASSERT(Ni == y.rows());
    ASSERT(Nj == x.rows());

    // multiplication
    auto A_tensor = make_sparse_csr_tensor(A);
    auto x_tensor = make_dense_tensor(x);
    auto y_tensor = tensor_to_host(torch::matmul(A_tensor, x_tensor));

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<Scalar>(), Ni * sizeof(Scalar));
}


void LinearAlgebraTorch::spmm(const SparseMatrix& A, const Matrix& X, Matrix& Y) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    auto Nk = static_cast<int64_t>(X.cols());
    ASSERT(Ni == Y.rows());
    ASSERT(Nj == X.rows());
    ASSERT(Nk == Y.cols());

    // multiplication and conversion from column-major to row-major (and back)
    auto A_tensor = make_sparse_csr_tensor(A);
    auto X_tensor = make_dense_tensor(X);
    auto Y_tensor = tensor_transpose(tensor_to_host(torch::matmul(A_tensor, X_tensor)));

    // assignment
    std::memcpy(Y.data(), Y_tensor.data_ptr<Scalar>(), Y.size() * sizeof(Scalar));
}


void LinearAlgebraTorch::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace eckit::linalg::sparse
