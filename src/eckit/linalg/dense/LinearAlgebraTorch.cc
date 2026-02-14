/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/dense/LinearAlgebraTorch.h"

#include <cstring>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"

#include "eckit/linalg/detail/Torch.h"


namespace eckit::linalg::dense {


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
using detail::torch_tensor_transpose;


Scalar LinearAlgebraTorch::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    auto x_tensor = make_torch_dense_tensor(x, get_torch_device(name()));
    auto y_tensor = make_torch_dense_tensor(y, get_torch_device(name()));

    return torch::dot(x_tensor, y_tensor).to(torch::kCPU).item<Scalar>();
}


void LinearAlgebraTorch::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(A.cols() == x.rows());
    ASSERT(A.rows() == y.rows());

    // multiplication
    auto A_tensor = make_torch_dense_tensor(A, get_torch_device(name()));
    auto x_tensor = make_torch_dense_tensor(x, get_torch_device(name()));
    auto y_tensor = torch::matmul(A_tensor, x_tensor).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<Scalar>(), y.rows() * sizeof(Scalar));
}


void LinearAlgebraTorch::gemm(const Matrix& A, const Matrix& X, Matrix& Y) const {
    ASSERT(A.cols() == X.rows());
    ASSERT(A.rows() == Y.rows());
    ASSERT(X.cols() == Y.cols());

    // multiplication and conversion from column-major to row-major (and back)
    auto A_tensor = make_torch_dense_tensor(A, get_torch_device(name()));
    auto X_tensor = make_torch_dense_tensor(X, get_torch_device(name()));
    auto Y_tensor = torch_tensor_transpose(torch::matmul(A_tensor, X_tensor)).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(Y.data(), Y_tensor.data_ptr<Scalar>(), Y.size() * sizeof(Scalar));
}


void LinearAlgebraTorch::print(std::ostream& out) const {
    out << "LinearAlgebraTorch[]";
}


}  // namespace eckit::linalg::dense
