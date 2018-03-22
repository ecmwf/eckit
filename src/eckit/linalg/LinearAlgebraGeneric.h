/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   LinearAlgebraGeneric.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_LinearAlgebraGeneric_h
#define eckit_la_LinearAlgebraGeneric_h

#include "eckit/linalg/LinearAlgebra.h"

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

class LinearAlgebraGeneric : public LinearAlgebra {

public:
    LinearAlgebraGeneric();

private:

    // Overridden methods

    virtual Scalar dot(const Vector&, const Vector&) const;
    virtual void gemv(const Matrix&, const Vector&, Vector&) const;
    virtual void gemm(const Matrix&, const Matrix&, Matrix&) const;
    virtual void spmv(const SparseMatrix&, const Vector&, Vector&) const;
    virtual void spmm(const SparseMatrix&, const Matrix&, Matrix&) const;
    virtual void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const;

    virtual void print(std::ostream&) const;

};

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif
