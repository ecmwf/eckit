/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


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
