/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include <string>

#include "eckit/linalg/types.h"

#include "torch/torch.h"


namespace eckit::linalg::detail {


torch::DeviceType get_torch_device(const std::string&);
torch::Tensor torch_tensor_transpose(const torch::Tensor&);
torch::Tensor make_torch_dense_tensor(const Matrix&, torch::DeviceType);
torch::Tensor make_torch_dense_tensor(const Vector&, torch::DeviceType);
torch::Tensor make_torch_sparse_csr(const SparseMatrix&, torch::DeviceType);


}  // namespace eckit::linalg::detail
