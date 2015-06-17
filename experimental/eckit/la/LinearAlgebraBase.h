/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   LinearAlgebraBase.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_LinearAlgebraBase_h
#define eckit_la_LinearAlgebraBase_h

#include "eckit/la/types.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

class LinearAlgebraBase {

public:  // virtual methods

    virtual Scalar dot(const Vector&, const Vector&) const = 0;
    virtual void gemv(const Matrix&, const Vector&, Vector&) const = 0;
    virtual void gemm(const Matrix&, const Matrix&, Matrix&) const = 0;
    virtual void spmv(const SparseMatrix&, const Vector&, Vector&) const = 0;
    virtual void spmm(const SparseMatrix&, const Matrix&, Matrix&) const = 0;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
