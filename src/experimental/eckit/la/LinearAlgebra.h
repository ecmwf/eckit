/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   LinearAlgebra.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_LinearAlgebra_h
#define eckit_la_LinearAlgebra_h

#include <string>

#include "experimental/eckit/la/types.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

class LinearAlgebra {

public:  // static methods
    static const LinearAlgebra& backend();
    static void backend(const std::string& name);
    static void list(std::ostream &);

public:  // virtual methods

    virtual Scalar dot(const Vector&, const Vector&) const = 0;
    virtual void gemv(const Matrix&, const Vector&, Vector&) const = 0;
    virtual void gemm(const Matrix&, const Matrix&, Matrix&) const = 0;
    virtual void spmv(const SparseMatrix&, const Vector&, Vector&) const = 0;
    virtual void spmm(const SparseMatrix&, const Matrix&, Matrix&) const = 0;

protected:
    LinearAlgebra(const std::string& name);
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
