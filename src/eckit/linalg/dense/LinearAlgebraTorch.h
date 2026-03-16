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
