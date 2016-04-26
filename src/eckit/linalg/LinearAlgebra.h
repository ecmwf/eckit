/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/memory/NonCopyable.h"
#include "eckit/linalg/types.h"


namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

class LinearAlgebra : private NonCopyable {

public:

    const std::string& name() const;

    // static methods

    /// Get the currently selected backend
    static const LinearAlgebra& backend();

    /// Select the given backend as the default
    static void backend(const std::string& name);

    /// List all available backends
    static void list(std::ostream &);

public:  // virtual methods

    /// Compute the inner product of vectors x and y
    virtual Scalar dot(const Vector&, const Vector&) const = 0;

    /// Compute the product of a dense matrix A and vector x. The caller is
    /// responsible for allocating a properly sized output vector y.
    virtual void gemv(const Matrix&, const Vector&, Vector&) const = 0;

    /// Compute the product of dense matrices A and B. The caller is
    /// responsible for allocating a properly sized output matrix C.
    virtual void gemm(const Matrix&, const Matrix&, Matrix&) const = 0;

    /// Compute the product of a sparse matrix A and vector x. The caller is
    /// responsible for allocating a properly sized output vector y.
    virtual void spmv(const SparseMatrix&, const Vector&, Vector&) const = 0;

    /// Compute the product of sparse matrix A and dense matrix B. The caller is
    /// responsible for allocating a properly sized output matrix C.
    virtual void spmm(const SparseMatrix&, const Matrix&, Matrix&) const = 0;

    /// Compute the product x A' y with x and y diagonal matrices stored as
    /// vectors and A a sparse matrix. The caller does NOT need to initialise
    /// the sparse output matrix C
    virtual void dsptd(const Vector&, const SparseMatrix&, const Vector&, SparseMatrix&) const;

protected:

    LinearAlgebra(const std::string& name);

    /// Get a backend by name
    static const LinearAlgebra& getBackend(const std::string& name);

private:

    std::string name_;

    virtual void print(std::ostream&) const = 0;

    // -- Friends

    friend std::ostream &operator<<(std::ostream &s, const LinearAlgebra &p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif
