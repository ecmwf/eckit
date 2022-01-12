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
/// @author Pedro Maciel


#pragma once

#include "eckit/eckit.h"

#ifdef eckit_HAVE_VIENNACL

#include <ostream>

#include "eckit/linalg/LinearAlgebra.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


namespace dense {


struct LinearAlgebraViennaCL final : public LinearAlgebraDense {
    LinearAlgebraViennaCL() {}
    LinearAlgebraViennaCL(const std::string& name) :
        LinearAlgebraDense(name) {}

    Scalar dot(const Vector&, const Vector&) const override;
    void gemv(const Matrix&, const Vector&, Vector&) const override;
    void gemm(const Matrix&, const Matrix&, Matrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace dense


//-----------------------------------------------------------------------------


namespace sparse {


struct LinearAlgebraViennaCL final : public LinearAlgebraSparse {
    LinearAlgebraViennaCL() {}
    LinearAlgebraViennaCL(const std::string& name) :
        LinearAlgebraSparse(name) {}

    void spmv(const SparseMatrix&, const Vector&, Vector&) const override;
    void spmm(const SparseMatrix&, const Matrix&, Matrix&) const override;
    void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace sparse


//-----------------------------------------------------------------------------


namespace deprecated {


class LinearAlgebraViennaCL final : public LinearAlgebra {
public:
    LinearAlgebraViennaCL(const std::string& name) :
        LinearAlgebra(name) {}

private:
    const dense::LinearAlgebraViennaCL lad_;
    const sparse::LinearAlgebraViennaCL las_;

    const LinearAlgebraDense& laDense() const override { return lad_; }
    const LinearAlgebraSparse& laSparse() const override { return las_; }

    void print(std::ostream& out) const override {
        out << "LinearAlgebraViennaCL[]";
    }
};


}  // namespace deprecated


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit


#endif  // eckit_HAVE_VIENNACL
