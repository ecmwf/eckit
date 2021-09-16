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

#include "eckit/eckit.h"

#include "eckit/linalg/LinearAlgebra.h"

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

class LinearAlgebraOpenMP : public LinearAlgebra {
public:
    LinearAlgebraOpenMP();

private:
    // Overridden methods

    Scalar dot(const Vector&, const Vector&) const;
    void gemv(const Matrix&, const Vector&, Vector&) const;
    void gemm(const Matrix&, const Matrix&, Matrix&) const;
    void spmv(const SparseMatrix&, const Vector&, Vector&) const;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const;

    void print(std::ostream&) const;
};

//-----------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit

