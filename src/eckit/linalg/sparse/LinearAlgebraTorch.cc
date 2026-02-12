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
#include <map>
#include <ostream>
#include <type_traits>

#include "torch/torch.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit::linalg::sparse {


static_assert(std::is_same<int32_t, Index>::value, "Index type mismatch");
static_assert(std::is_same<double, Scalar>::value, "Scalar type mismatch");


namespace {


const LinearAlgebraTorch LA_TORCH_CPU_1("torch");
const LinearAlgebraTorch LA_TORCH_CPU_2("torch-cpu");
const LinearAlgebraTorch LA_TORCH_CUDA("torch-cuda");
const LinearAlgebraTorch LA_TORCH_HIP("torch-hip");
const LinearAlgebraTorch LA_TORCH_MPS("torch-mps");
const LinearAlgebraTorch LA_TORCH_XPU("torch-xpu");
const LinearAlgebraTorch LA_TORCH_XLA("torch-xla");
const LinearAlgebraTorch LA_TORCH_META("torch-meta");


torch::DeviceType get_device(const std::string& name) {
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
        return sep != std::string::npos ? types.at(name.substr(sep + 1)) : torch::DeviceType::CPU;
    }();

    return device;
}


torch::Tensor make_sparse_csr(const SparseMatrix& A, torch::DeviceType device) {
    auto Ni  = static_cast<int64_t>(A.rows());
    auto Nj  = static_cast<int64_t>(A.cols());
    auto Nz  = static_cast<int64_t>(A.nonZeros());

    auto ia = torch::from_blob(const_cast<Index*>(A.outer()), {Ni + 1}, torch::kInt32).to(device, torch::kInt64);
    auto ja = torch::from_blob(const_cast<Index*>(A.inner()), {Nz}, torch::kInt32).to(device, torch::kInt64);
    auto a  = torch::from_blob(const_cast<Scalar*>(A.data()), {Nz}, torch::kFloat64).to(device);

    return torch::sparse_csr_tensor(
        ia, ja, a, {Ni, Nj}, torch::TensorOptions().dtype(torch::kFloat64).device(device).layout(torch::kSparseCsr));
}


}  // namespace


void LinearAlgebraTorch::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    auto Ni = static_cast<int64_t>(A.rows());
    auto Nj = static_cast<int64_t>(A.cols());
    ASSERT(Ni == y.rows());
    ASSERT(Nj == x.rows());

    // multiplication
    auto A_tensor = make_sparse_csr(A, get_device(name()));
    auto x_tensor = torch::from_blob(const_cast<Scalar*>(x.data()), {Nj}, torch::kFloat64).to(get_device(name()));
    auto y_tensor = torch::matmul(A_tensor, x_tensor).to(torch::kCPU).contiguous();

    // assignment
    std::memcpy(y.data(), y_tensor.data_ptr<Scalar>(), Ni * sizeof(Scalar));
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
    auto A_tensor = make_sparse_csr(A, get_device(name()));
    auto X_tensor =
        transpose(torch::from_blob(const_cast<Scalar*>(X.data()), {Nk, Nj}, torch::kFloat64).to(get_device(name())));
    auto Y_tensor = transpose(torch::matmul(A_tensor, X_tensor)).to(torch::kCPU).contiguous();

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
