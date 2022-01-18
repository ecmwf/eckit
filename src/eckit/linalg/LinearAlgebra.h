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

#include <iosfwd>
#include <string>

#include "eckit/linalg/LinearAlgebraDense.h"
#include "eckit/linalg/LinearAlgebraSparse.h"
#include "eckit/linalg/types.h"


namespace eckit {
namespace linalg {


//-----------------------------------------------------------------------------


/// @deprecated Use LinearAlgebraDense/LinearAlgebraSparse instead
class LinearAlgebra {
public:
    // - Static methods

    /// Get the currently selected backend
    static const LinearAlgebra& backend();

    /// Select the given backend as the default
    static void backend(const std::string& name);

    /// List all available backends
    static std::ostream& list(std::ostream&);

    /// Get a backend by name
    static const LinearAlgebra& getBackend(const std::string& name);

    /// Check if a backend is available
    static bool hasBackend(const std::string& name);

    // - Methods

    /// Return active backend name
    const std::string& name() const {
        return name_;
    }

    /// Compute the inner product of vectors x and y
    Scalar dot(const Vector& x, const Vector& y) const {
        return laDense().dot(x, y);
    }

    /// Compute the product of a dense matrix A and vector x
    /// @note y must be allocated and sized correctly
    void gemv(const Matrix& A, const Vector& x, Vector& y) const {
        laDense().gemv(A, x, y);
    }

    /// Compute the product of dense matrices A and X
    /// @note Y must be allocated and sized correctly
    void gemm(const Matrix& A, const Matrix& X, Matrix& Y) const {
        laDense().gemm(A, X, Y);
    }

    /// Compute the product of a sparse matrix A and vector x
    /// @note y must be allocated and sized correctly
    void spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
        laSparse().spmv(A, x, y);
    }

    /// Compute the product of sparse matrix A and dense matrix X
    /// @note Y must be allocated and sized correctly
    void spmm(const SparseMatrix& A, const Matrix& X, Matrix& Y) const {
        laSparse().spmm(A, X, Y);
    }

    /// Compute the product x A' y with x and y diagonal matrices stored as
    /// vectors and A a sparse matrix
    /// @note B does NOT need to be allocated/sized correctly
    void dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
        laSparse().dsptd(x, A, y, B);
    }

protected:
    LinearAlgebra(const std::string& name);

    virtual ~LinearAlgebra() = default;

private:
    std::string name_;

    virtual const LinearAlgebraDense& laDense() const   = 0;
    virtual const LinearAlgebraSparse& laSparse() const = 0;
    virtual void print(std::ostream&) const             = 0;

    LinearAlgebra(const LinearAlgebra&) = delete;
    LinearAlgebra& operator=(const LinearAlgebra&) = delete;

    friend std::ostream& operator<<(std::ostream& s, const LinearAlgebra& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
