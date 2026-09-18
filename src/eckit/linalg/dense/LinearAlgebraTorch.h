// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/linalg/LinearAlgebraDense.h"
#include "eckit/linalg/detail/Torch.h"


namespace eckit::linalg::dense {


struct LinearAlgebraTorch final : public LinearAlgebraDense, detail::Torch {
    LinearAlgebraTorch(const std::string& name, torch::DeviceType device, torch::ScalarType scalar = torch::kFloat64) :
        LinearAlgebraDense(name), Torch(device, scalar) {}

    Scalar dot(const Vector& x, const Vector& y) const override;
    void gemv(const Matrix& A, const Vector& x, Vector& y) const override;
    void gemm(const Matrix& A, const Matrix& X, Matrix& Y) const override;
    void print(std::ostream& os) const override { Torch::print(os); }
};


}  // namespace eckit::linalg::dense
