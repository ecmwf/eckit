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

#include <ostream>
#include <type_traits>

#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit::linalg::detail {


static_assert(std::is_same<int32_t, Index>::value, "Index type mismatch");
static_assert(std::is_same<double, Scalar>::value, "Scalar type mismatch");


torch::Tensor Torch::tensor_transpose(const torch::Tensor& tensor) const {
    return tensor.transpose(0, 1).contiguous();
}


torch::Tensor Torch::tensor_to_host(const torch::Tensor& tensor) const {
    return tensor.to(torch::DeviceType::CPU, torch::kFloat64).contiguous();  // reverse MPS float32 (if applicable)
}


torch::Tensor Torch::make_dense_tensor(const Matrix& A) const {
    auto Ni = static_cast<int64_t>(A.cols());
    auto Nj = static_cast<int64_t>(A.rows());

    return tensor_transpose(
        torch::from_blob(const_cast<Scalar*>(A.data()), {Ni, Nj}, torch::kFloat64).to(device_, scalar_));
}


torch::Tensor Torch::make_dense_tensor(const Vector& V) const {
    auto Ni = static_cast<int64_t>(V.size());

    return torch::from_blob(const_cast<Scalar*>(V.data()), {Ni}, torch::kFloat64).to(device_, scalar_);
}


torch::Tensor Torch::make_sparse_csr_tensor(const SparseMatrix& A) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    auto Nz = static_cast<int64_t>(A.nonZeros());

    auto ia = torch::from_blob(const_cast<Index*>(A.outer()), {Ni + 1}, torch::kInt32).to(device_, torch::kInt64);
    auto ja = torch::from_blob(const_cast<Index*>(A.inner()), {Nz}, torch::kInt32).to(device_, torch::kInt64);
    auto a  = torch::from_blob(const_cast<Scalar*>(A.data()), {Nz}, torch::kFloat64).to(device_, scalar_);

    return torch::sparse_csr_tensor(ia, ja, a, {Ni, Nj},
                                    torch::TensorOptions().dtype(scalar_).device(device_).layout(torch::kSparseCsr));
}


void Torch::print(std::ostream& os) const {
    os << "LinearAlgebraTorch[device=" << device_ << "]";
}


}  // namespace eckit::linalg::detail
