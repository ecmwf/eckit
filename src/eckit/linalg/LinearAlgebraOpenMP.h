/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Pedro Maciel


#pragma once

#include <ostream>

#include "eckit/linalg/LinearAlgebra.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


namespace dense {


struct LinearAlgebraOpenMP final : public LinearAlgebraDense {
    LinearAlgebraOpenMP() {}
    LinearAlgebraOpenMP(const std::string& name) :
        LinearAlgebraDense(name) {}

    Scalar dot(const Vector&, const Vector&) const override;
    void gemv(const Matrix&, const Vector&, Vector&) const override;
    void gemm(const Matrix&, const Matrix&, Matrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace dense


//-----------------------------------------------------------------------------


namespace sparse {


struct LinearAlgebraOpenMP final : public LinearAlgebraSparse {
    LinearAlgebraOpenMP() {}
    LinearAlgebraOpenMP(const std::string& name) :
        LinearAlgebraSparse(name) {}

    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace sparse


//-----------------------------------------------------------------------------


namespace deprecated {


class LinearAlgebraOpenMP final : public LinearAlgebra {
public:
    LinearAlgebraOpenMP(const std::string& name) :
        LinearAlgebra(name) {}

private:
    const dense::LinearAlgebraOpenMP lad_;
    const sparse::LinearAlgebraOpenMP las_;

    const LinearAlgebraDense& laDense() const override { return lad_; }
    const LinearAlgebraSparse& laSparse() const override { return las_; }

    void print(std::ostream& out) const override {
        out << "LinearAlgebraOpenMP[]";
    }
};


}  // namespace deprecated


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
