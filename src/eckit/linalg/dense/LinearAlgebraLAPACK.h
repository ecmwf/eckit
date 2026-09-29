// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/linalg/LinearAlgebraDense.h"

namespace eckit::linalg::dense {

struct LinearAlgebraLAPACK final : public LinearAlgebraDense {
    LinearAlgebraLAPACK() {}
    LinearAlgebraLAPACK(const std::string& name) : LinearAlgebraDense(name) {}

    Scalar dot(const Vector&, const Vector&) const override;
    void gemv(const Matrix&, const Vector&, Vector&) const override;
    void gemm(const Matrix&, const Matrix&, Matrix&) const override;
    void print(std::ostream&) const override;
};

}  // namespace eckit::linalg::dense
