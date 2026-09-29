// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/linalg/LinearAlgebraSparse.h"
#include "eckit/linalg/detail/Torch.h"


namespace eckit::linalg::sparse {


struct LinearAlgebraTorch final : public LinearAlgebraSparse, detail::Torch {
    LinearAlgebraTorch(const std::string& name, torch::DeviceType device, torch::ScalarType scalar = torch::kFloat64) :
        LinearAlgebraSparse(name), Torch(device, scalar) {}

    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;
    void print(std::ostream& os) const override { Torch::print(os); }
};


}  // namespace eckit::linalg::sparse
