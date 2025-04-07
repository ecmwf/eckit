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

#include "eckit/linalg/types.h"

namespace eckit::linalg {

//-----------------------------------------------------------------------------

class LinearAlgebraDense {
public:

    // - Static methods

    /// Get backend, re-setting default
    static const LinearAlgebraDense& backend(const std::string& name = "");

    /// Get backend
    static const LinearAlgebraDense& getBackend(const std::string& name);

    /// Check if a backend is available
    static bool hasBackend(const std::string& name);

    /// List all available backends
    static std::ostream& list(std::ostream&);

    /// Return active backend name
    static const std::string& name();

    // - Methods

    /// Compute the inner product of vectors x and y
    virtual Scalar dot(const Vector& x, const Vector& y) const = 0;

    /// Compute the product of a dense matrix A and vector x
    /// @note y must be allocated and sized correctly
    virtual void gemv(const Matrix& A, const Vector& x, Vector& y) const = 0;

    /// Compute the product of dense matrices A and X
    /// @note Y must be allocated and sized correctly
    virtual void gemm(const Matrix& A, const Matrix& X, Matrix& Y) const = 0;

protected:

    LinearAlgebraDense() = default;
    LinearAlgebraDense(const std::string& name);

    virtual ~LinearAlgebraDense() = default;

private:

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const LinearAlgebraDense& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit::linalg
