/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/detail/Torch.h"

#include <map>
#include <type_traits>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit::linalg::detail {


static_assert(std::is_same<int32_t, Index>::value, "Index type mismatch");
static_assert(std::is_same<double, Scalar>::value, "Scalar type mismatch");


torch::DeviceType get_torch_device(const std::string& name) {
    static const auto device = [&name]() {
        const std::map<std::string, torch::DeviceType> types{
            {"cpu", torch::DeviceType::CPU},    //
            {"cuda", torch::DeviceType::CUDA},  //
            {"hip", torch::DeviceType::HIP},    //
            {"mps", torch::DeviceType::MPS},    //
            {"xpu", torch::DeviceType::XPU},    //
            {"xla", torch::DeviceType::XLA},    //
            {"meta", torch::DeviceType::Meta},  //
        };

        const auto sep = name.find_first_of('-');
        if (sep == std::string::npos) {
            return torch::DeviceType::CPU;
        }

        if (auto it = types.find(name.substr(sep + 1)); it != types.end()) {
            return it->second;
        }

        throw eckit::UserError("Unknown torch device: " + name);
    }();

    return device;
}


torch::Tensor torch_tensor_transpose(const torch::Tensor& tensor) {
    return tensor.transpose(0, 1).contiguous();
}


torch::Tensor make_torch_dense_tensor(const Matrix& A, torch::DeviceType device) {
    auto Ni = static_cast<int64_t>(A.cols());
    auto Nj = static_cast<int64_t>(A.rows());

    return torch_tensor_transpose(
        torch::from_blob(const_cast<Scalar*>(A.data()), {Ni, Nj}, torch::kFloat64).to(device));
}


torch::Tensor make_torch_dense_tensor(const Vector& V, torch::DeviceType device) {
    auto Ni = static_cast<int64_t>(V.size());

    return torch::from_blob(const_cast<Scalar*>(V.data()), {Ni}, torch::kFloat64).to(device);
}


torch::Tensor make_torch_sparse_csr(const SparseMatrix& A, torch::DeviceType device) {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    auto Nz = static_cast<int64_t>(A.nonZeros());

    auto ia = torch::from_blob(const_cast<Index*>(A.outer()), {Ni + 1}, torch::kInt32).to(device, torch::kInt64);
    auto ja = torch::from_blob(const_cast<Index*>(A.inner()), {Nz}, torch::kInt32).to(device, torch::kInt64);
    auto a  = torch::from_blob(const_cast<Scalar*>(A.data()), {Nz}, torch::kFloat64).to(device);

    return torch::sparse_csr_tensor(
        ia, ja, a, {Ni, Nj}, torch::TensorOptions().dtype(torch::kFloat64).device(device).layout(torch::kSparseCsr));
}


}  // namespace eckit::linalg::detail
