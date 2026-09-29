// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/linalg/dense/LinearAlgebraTorch.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/detail/Torch.h"


namespace eckit::linalg::dense {


static const LinearAlgebraTorch LA_TORCH_CPU_1("torch", torch::DeviceType::CPU);
static const LinearAlgebraTorch LA_TORCH_CPU_2("torch-cpu", torch::DeviceType::CPU);
static const LinearAlgebraTorch LA_TORCH_CUDA("torch-cuda", torch::DeviceType::CUDA);
static const LinearAlgebraTorch LA_TORCH_HIP("torch-hip", torch::DeviceType::HIP);
static const LinearAlgebraTorch LA_TORCH_MPS("torch-mps", torch::DeviceType::MPS, torch::kFloat32);
static const LinearAlgebraTorch LA_TORCH_XPU("torch-xpu", torch::DeviceType::XPU);
static const LinearAlgebraTorch LA_TORCH_XLA("torch-xla", torch::DeviceType::XLA);
static const LinearAlgebraTorch LA_TORCH_META("torch-meta", torch::DeviceType::Meta);


Scalar LinearAlgebraTorch::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    auto x_tensor = make_dense_tensor(x);
    auto y_tensor = make_dense_tensor(y);

    return tensor_to_host(torch::dot(x_tensor, y_tensor)).item<Scalar>();
}


void LinearAlgebraTorch::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(A.cols() == x.rows());
    ASSERT(A.rows() == y.rows());

    // multiplication
    auto A_tensor = make_dense_tensor(A);
    auto x_tensor = make_dense_tensor(x);
    auto y_tensor = tensor_to_host(torch::matmul(A_tensor, x_tensor));

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<Scalar>(), y.rows() * sizeof(Scalar));
}


void LinearAlgebraTorch::gemm(const Matrix& A, const Matrix& X, Matrix& Y) const {
    ASSERT(A.cols() == X.rows());
    ASSERT(A.rows() == Y.rows());
    ASSERT(X.cols() == Y.cols());

    // multiplication and conversion from column-major to row-major (and back)
    auto A_tensor = make_dense_tensor(A);
    auto X_tensor = make_dense_tensor(X);
    auto Y_tensor = tensor_transpose(tensor_to_host(torch::matmul(A_tensor, X_tensor)));

    // assignment
    std::memcpy(Y.data(), Y_tensor.data_ptr<Scalar>(), Y.size() * sizeof(Scalar));
}


}  // namespace eckit::linalg::dense
