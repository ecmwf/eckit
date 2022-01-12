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

#ifdef eckit_HAVE_ARMADILLO

#include <ostream>

#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


namespace dense {


struct LinearAlgebraArmadillo final : public LinearAlgebraDense {
    LinearAlgebraArmadillo() {}
    LinearAlgebraArmadillo(const std::string& name) :
        LinearAlgebraDense(name) {}

    Scalar dot(const Vector&, const Vector&) const override;
    void gemv(const Matrix&, const Vector&, Vector&) const override;
    void gemm(const Matrix&, const Matrix&, Matrix&) const override;
    void print(std::ostream&) const override;
};


}  // namespace dense


//-----------------------------------------------------------------------------


namespace deprecated {


class LinearAlgebraArmadillo final : public LinearAlgebra {
public:
    LinearAlgebraArmadillo(const std::string& name) :
        LinearAlgebra(name) {}

private:
    const dense::LinearAlgebraArmadillo lad_;

    // FIXME Armadillo stores matrices in CSC format and does not provide
    // constructors from existing storage. A sparse matrix would have to be
    // copied from CSR to CSC format, which is probably not worth the cost
    const sparse::LinearAlgebraGeneric las_;

    const LinearAlgebraDense& laDense() const override { return lad_; }
    const LinearAlgebraSparse& laSparse() const override { return las_; }

    void print(std::ostream& out) const override {
        out << "LinearAlgebraArmadillo[]";
    }
};


}  // namespace deprecated


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit


#endif  // eckit_HAVE_ARMADILLO
