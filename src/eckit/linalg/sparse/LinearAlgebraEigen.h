// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/linalg/LinearAlgebraSparse.h"

namespace eckit::linalg::sparse {

struct LinearAlgebraEigen final : public LinearAlgebraSparse {
    LinearAlgebraEigen() {}
    LinearAlgebraEigen(const std::string& name) : LinearAlgebraSparse(name) {}

    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;
    void print(std::ostream&) const override;
};

}  // namespace eckit::linalg::sparse
