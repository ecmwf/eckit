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
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"

#include "eckit/linalg/detail/Torch.h"


namespace eckit::linalg::sparse {


static const LinearAlgebraTorch LA_TORCH_CPU_1("torch");
static const LinearAlgebraTorch LA_TORCH_CPU_2("torch-cpu");
static const LinearAlgebraTorch LA_TORCH_CUDA("torch-cuda");
static const LinearAlgebraTorch LA_TORCH_HIP("torch-hip");
static const LinearAlgebraTorch LA_TORCH_MPS("torch-mps");
static const LinearAlgebraTorch LA_TORCH_XPU("torch-xpu");
static const LinearAlgebraTorch LA_TORCH_XLA("torch-xla");
static const LinearAlgebraTorch LA_TORCH_META("torch-meta");


using detail::get_torch_device;
using detail::make_torch_dense_tensor;
using detail::make_torch_sparse_csr;
using detail::torch_tensor_transpose;


void LinearAlgebraTorch::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    ASSERT(Ni == y.rows());
    ASSERT(Nj == x.rows());

    // Note: This implementation copies data to GPU memory for each operation and immediately
    // copies the result back to CPU. This data transfer overhead can be significant and may
    // negate the performance benefits of GPU computation for small matrices or frequent operations.
    // GPU acceleration is most beneficial for large matrices where computation time dominates
    // transfer overhead. For optimal performance, consider keeping data on GPU across multiple
    // operations rather than transferring for each call.

    // multiplication
    auto A_tensor = make_torch_sparse_csr(A, get_torch_device(name()));
    auto x_tensor = make_torch_dense_tensor(x, get_torch_device(name()));
    auto y_tensor = torch::matmul(A_tensor, x_tensor).to(torch::kCPU).contiguous();

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

    // Note: This implementation copies data to GPU memory for each operation and immediately
    // copies the result back to CPU. This data transfer overhead can be significant and may
    // negate the performance benefits of GPU computation for small matrices or frequent operations.
    // GPU acceleration is most beneficial for large matrices where computation time dominates
    // transfer overhead. For optimal performance, consider keeping data on GPU across multiple
    // operations rather than transferring for each call.

    // multiplication and conversion from column-major to row-major (and back)
    auto A_tensor = make_torch_sparse_csr(A, get_torch_device(name()));
    auto X_tensor = make_torch_dense_tensor(X, get_torch_device(name()));
    auto Y_tensor = torch_tensor_transpose(torch::matmul(A_tensor, X_tensor)).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(Y.data(), Y_tensor.data_ptr<Scalar>(), Y.size() * sizeof(Scalar));
}


void LinearAlgebraTorch::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


void LinearAlgebraTorch::print(std::ostream& out) const {
    out << "LinearAlgebraTorch[]";
}


}  // namespace eckit::linalg::sparse
