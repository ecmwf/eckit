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
