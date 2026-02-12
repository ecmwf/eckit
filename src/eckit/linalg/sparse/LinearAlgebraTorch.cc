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
#include <type_traits>

#include "torch/torch.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit::linalg::sparse {


static_assert(std::is_same<int32_t, Index>::value, "Index type mismatch");
static_assert(std::is_same<double, Scalar>::value, "Scalar type mismatch");

static const LinearAlgebraTorch __la_torch("torch");


namespace {


torch::DeviceType get_device() {
    static const auto _device = [](const std::string& dev) {
        auto device = dev == "cpu"      ? torch::DeviceType::CPU
                      : dev == "cuda"   ? torch::DeviceType::CUDA
                      : dev == "hip"    ? torch::DeviceType::HIP
                      : dev == "fpga"   ? torch::DeviceType::FPGA
                      : dev == "maia"   ? torch::DeviceType::MAIA
                      : dev == "xla"    ? torch::DeviceType::XLA
                      : dev == "mps"    ? torch::DeviceType::MPS
                      : dev == "meta"   ? torch::DeviceType::Meta
                      : dev == "vulkan" ? torch::DeviceType::Vulkan
                      : dev == "metal"  ? torch::DeviceType::Metal
                      : dev == "xpu"    ? torch::DeviceType::XPU
                      : dev == "hpu"    ? torch::DeviceType::HPU
                      : dev == "ve"     ? torch::DeviceType::VE
                      : dev == "lazy"   ? torch::DeviceType::Lazy
                      : dev == "ipu"    ? torch::DeviceType::IPU
                      : dev == "mtia"   ? torch::DeviceType::MTIA
                                        : NOTIMP;
        return device == torch::DeviceType::MPS ? torch::DeviceType::CPU : device;
    }(eckit::Resource<std::string>("$ECKIT_LINALG_TORCH_DEVICE;eckitLinalgTorchDevice;-eckitLinalgTorchDevice", "cpu"));

    // Note: MPS doesn't currently support sparse tensors, revert to CPU
    static const auto _safe_device = _device == torch::DeviceType::MPS ? torch::DeviceType::CPU : _device;

    return _safe_device;
}


torch::Tensor make_sparse_csr(const SparseMatrix& A) {
    auto dev = get_device();
    auto Ni  = static_cast<int64_t>(A.rows());
    auto Nj  = static_cast<int64_t>(A.cols());
    auto Nz  = static_cast<int64_t>(A.nonZeros());

    auto ia = torch::from_blob(A.outer(), {Ni + 1}, torch::kInt32).to(dev, torch::kInt64);
    auto ja = torch::from_blob(A.inner(), {Nz}, torch::kInt32).to(dev, torch::kInt64);
    auto a  = torch::from_blob(const_cast<double*>(A.data()), {Nz}, torch::kFloat64).to(dev);

    return torch::sparse_csr_tensor(
        ia, ja, a, {Ni, Nj}, torch::TensorOptions().dtype(torch::kFloat64).device(dev).layout(torch::kSparseCsr));
}


}  // namespace


void LinearAlgebraTorch::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    ASSERT(Ni == y.rows());
    ASSERT(Nj == x.rows());

    // multiplication
    auto A_tensor = make_sparse_csr(A);
    auto x_tensor = torch::from_blob(const_cast<double*>(x.data()), {Nj}, torch::kFloat64).to(get_device());
    auto y_tensor = torch::matmul(A_tensor, x_tensor).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<double>(), Ni * sizeof(double));
}


void LinearAlgebraTorch::spmm(const SparseMatrix& A, const Matrix& X, Matrix& Y) const {
    auto transpose = [](const auto& tensor) { return tensor.transpose(0, 1).contiguous(); };

    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    auto Nk = static_cast<int64_t>(X.cols());
    ASSERT(Ni == Y.rows());
    ASSERT(Nj == X.rows());
    ASSERT(Nk == Y.cols());

    // multiplication and conversion from column-major to row-major (and back)
    auto A_tensor = make_sparse_csr(A);
    auto X_tensor =
        transpose(torch::from_blob(const_cast<double*>(X.data()), {Nk, Nj}, torch::kFloat64).to(get_device()));
    auto Y_tensor = transpose(torch::matmul(A_tensor, X_tensor)).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(Y.data(), Y_tensor.data_ptr<double>(), Y.size() * sizeof(double));
}


void LinearAlgebraTorch::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


void LinearAlgebraTorch::print(std::ostream& out) const {
    out << "LinearAlgebraTorch[]";
}


}  // namespace eckit::linalg::sparse
