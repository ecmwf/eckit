// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/linalg/LinearAlgebraSparse.h"


namespace eckit {
namespace linalg {
namespace sparse {


struct LinearAlgebraHIP final : public LinearAlgebraSparse {
    LinearAlgebraHIP() {}
    LinearAlgebraHIP(const std::string& name) : LinearAlgebraSparse(name) {}

    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
