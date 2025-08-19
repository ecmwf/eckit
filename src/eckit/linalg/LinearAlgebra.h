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

#include <ostream>
#include <string>

#include "eckit/deprecated.h"
#include "eckit/linalg/LinearAlgebraDense.h"
#include "eckit/linalg/LinearAlgebraSparse.h"
#include "eckit/linalg/types.h"

namespace eckit::linalg {

//-----------------------------------------------------------------------------

class LinearAlgebra {
public:

    // - Static methods

    /// Get the currently selected backend (instance)
    static const LinearAlgebra& backend();

    /// Select specific backend by name (re-setting default)
    DEPRECATED("Use LinearAlgebraDense/LinearAlgebraSparse::backend() instead")
    static void backend(const std::string& name);

    /// List all available backends
    static std::ostream& list(std::ostream&);

    /// Get specific backend by name
    DEPRECATED("Use LinearAlgebraDense/LinearAlgebraSparse::getBackend() instead")
    static const LinearAlgebra& getBackend(const std::string& name);

    /// Check if a backend is available
    static bool hasBackend(const std::string& name);

    /// Get current or specific LinearAlgebraDense backend
    static const LinearAlgebraDense& getDenseBackend(const std::string& name = "");

    /// Get current or specific LinearAlgebraSparse backend
    static const LinearAlgebraSparse& getSparseBackend(const std::string& name = "");

    /// Get current or specific LinearAlgebraDense backend (re-setting default)
    static const LinearAlgebraDense& denseBackend(const std::string& name = "");

    /// Get current or specific LinearAlgebraSparse backend (re-setting default)
    static const LinearAlgebraSparse& sparseBackend(const std::string& name = "");

    /// Check if a LinearAlgebraDense backend is available
    static bool hasDenseBackend(const std::string& name);

    /// Check if a LinearAlgebraSparse backend is available
    static bool hasSparseBackend(const std::string& name);

    /// Return active backend name
    DEPRECATED("Use LinearAlgebraDense/LinearAlgebraSparse::name() instead")
    static std::string name();

    /// Compute the inner product of vectors x and y
    static Scalar dot(const Vector& x, const Vector& y) { return LinearAlgebraDense::backend().dot(x, y); }

    /// Compute the product of a dense matrix A and vector x
    /// @note y must be allocated and sized correctly
    static void gemv(const Matrix& A, const Vector& x, Vector& y) { LinearAlgebraDense::backend().gemv(A, x, y); }

    /// Compute the product of dense matrices A and X
    /// @note Y must be allocated and sized correctly
    static void gemm(const Matrix& A, const Matrix& X, Matrix& Y) { LinearAlgebraDense::backend().gemm(A, X, Y); }

    /// Compute the product of a sparse matrix A and vector x
    /// @note y must be allocated and sized correctly
    static void spmv(const SparseMatrix& A, const Vector& x, Vector& y) {
        LinearAlgebraSparse::backend().spmv(A, x, y);
    }

    /// Compute the product of sparse matrix A and dense matrix X
    /// @note Y must be allocated and sized correctly
    static void spmm(const SparseMatrix& A, const Matrix& X, Matrix& Y) {
        LinearAlgebraSparse::backend().spmm(A, X, Y);
    }

    /// Compute the product x A' y with x and y diagonal matrices stored as
    /// vectors and A a sparse matrix
    /// @note B does NOT need to be allocated/sized correctly
    static void dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) {
        LinearAlgebraSparse::backend().dsptd(x, A, y, B);
    }

protected:

    LinearAlgebra() = default;

private:

    LinearAlgebra(const LinearAlgebra&)            = delete;
    LinearAlgebra& operator=(const LinearAlgebra&) = delete;

    friend std::ostream& operator<<(std::ostream& s, const LinearAlgebra&) {
        return s << "LinearAlgebra[LinearAlgebraDense=[" << LinearAlgebraDense::backend() << "],LinearAlgebraSparse=["
                 << LinearAlgebraSparse::backend() << "]]";
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit::linalg
