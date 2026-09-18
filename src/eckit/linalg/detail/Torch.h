// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <iosfwd>

#include "eckit/linalg/types.h"

#include "torch/torch.h"


namespace eckit::linalg::detail {


/**
 * @brief Torch tensor creation and device management for linear algebra backends.
 *
 * Copies data host to/from device per operation. Transfer overhead may outweigh accelerator device gains for
 * small/frequent operations; best suited for large matrices where compute dominates.
 */
class Torch {
protected:

    explicit Torch(torch::DeviceType device, torch::ScalarType scalar) : device_(device), scalar_(scalar) {}

    torch::Tensor tensor_transpose(const torch::Tensor&) const;
    torch::Tensor tensor_to_host(const torch::Tensor&) const;

    torch::Tensor make_dense_tensor(const Matrix&) const;
    torch::Tensor make_dense_tensor(const Vector&) const;
    torch::Tensor make_sparse_csr_tensor(const SparseMatrix&) const;

    void print(std::ostream&) const;

private:

    const torch::DeviceType device_;
    const torch::ScalarType scalar_;
};


}  // namespace eckit::linalg::detail
