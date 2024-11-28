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


torch::TensorOptions make_options(torch::ScalarType _dtype) {
    static const auto _device = [](const std::string& dev) {
        return dev == "cpu"      ? torch::DeviceType::CPU
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
    }(eckit::Resource<std::string>("$ECKIT_LINALG_TORCH_DEVICE;eckitLinalgTorchDevice;-eckitLinalgTorchDevice", "cpu"));

    return torch::TensorOptions().dtype(_dtype).device(_device);
}


}  // namespace


void LinearAlgebraTorch::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    const auto options_int   = make_options(torch::kInt32);
    const auto options_float = make_options(torch::kFloat64);

    const auto Ni = static_cast<int32_t>(A.rows());
    const auto Nj = static_cast<int32_t>(A.cols());
    const auto Nz = static_cast<int32_t>(A.nonZeros());
    ASSERT(Ni == y.rows());
    ASSERT(Nj == x.rows());

    // torch tensors
    const auto ia = torch::from_blob(const_cast<int32_t*>(A.outer()), Ni + 1, options_int);
    const auto ja = torch::from_blob(const_cast<int32_t*>(A.inner()), Nz, options_int);
    const auto a  = torch::from_blob(const_cast<double*>(A.data()), Nz, options_float);

    const auto A_tensor = torch::sparse_csr_tensor(ia, ja, a, {Ni, Nj}, options_float.layout(torch::kSparseCsr));

    // multiplication
    const auto x_tensor = torch::from_blob(const_cast<double*>(x.data()), Nj, options_float);
    const auto y_tensor = torch::matmul(A_tensor, x_tensor);

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<double>(), Ni * sizeof(double));
}


void LinearAlgebraTorch::spmm(const SparseMatrix& A, const Matrix& X, Matrix& Y) const {
    const auto options_int   = make_options(torch::kInt32);
    const auto options_float = make_options(torch::kFloat64);

    const auto Ni = static_cast<int32_t>(A.rows());
    const auto Nj = static_cast<int32_t>(A.cols());
    const auto Nk = static_cast<int32_t>(X.cols());
    const auto Nz = static_cast<int32_t>(A.nonZeros());
    ASSERT(Ni == Y.rows());
    ASSERT(Nj == X.rows());
    ASSERT(Nk == Y.cols());

    // torch tensors
    const auto ia = torch::from_blob(const_cast<int32_t*>(A.outer()), Ni + 1, options_int);
    const auto ja = torch::from_blob(const_cast<int32_t*>(A.inner()), Nz, options_int);
    const auto a  = torch::from_blob(const_cast<double*>(A.data()), Nz, options_float);

    const auto A_tensor = torch::sparse_csr_tensor(ia, ja, a, {Ni, Nj}, options_float.layout(torch::kSparseCsr));

    // multiplication and conversion from column-major to row-major (and back)
    auto t = [](auto&& tensor) { return tensor.transpose(0, 1).contiguous(); };

    const auto X_tensor = t(torch::from_blob(const_cast<double*>(X.data()), {Nk, Nj}, options_float));
    const auto Y_tensor = t(torch::matmul(A_tensor, X_tensor));

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
