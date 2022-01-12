/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Florian Rathgeber

#pragma once

#include "eckit/linalg/LinearAlgebra.h"

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

class LinearAlgebraGeneric final : public LinearAlgebra {

public:
    LinearAlgebraGeneric();

private:
    // Overridden methods

    Scalar dot(const Vector&, const Vector&) const override;
    void gemv(const Matrix&, const Vector&, Vector&) const override;
    void gemm(const Matrix&, const Matrix&, Matrix&) const override;
    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;

    void print(std::ostream&) const override;
};

//-----------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
