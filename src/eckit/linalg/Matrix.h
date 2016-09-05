/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Matrix.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_linalg_Matrix_h
#define eckit_linalg_Matrix_h

#include <vector>
#include "eckit/linalg/types.h"

namespace eckit {
class Stream;
}

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

/// Dense matrix in column major storage order
class Matrix {
public:  // types
    typedef eckit::linalg::Size Size;

public:  // methods

    // -- Constructors

    /// Default constructor (empty matrix)
    Matrix();

    /// Construct matrix with given rows and columns (allocates memory)
    Matrix(Size rows, Size cols);

    /// Construct matrix from existing data (does NOT take ownership)
    Matrix(Scalar* m, Size rows, Size cols);

    /// Constructor from Stream
    Matrix(Stream&);

    /// Copy constructor
    Matrix(const Matrix&);

    // TODO: make virtual if used as base class
    ~Matrix();

    // -- Mutators

    Matrix& operator=(const Matrix&);

    /// Swap this matrix with another
    void swap(Matrix&);

    /// Resize matrix to given number of rows/columns (invalidates data)
    void resize(Size rows, Size cols);

    /// Set data to zero
    void setZero();

    /// Fill vector with given scalar
    void fill(Scalar);
    // -- Serialisation

    /// Serialise to a Stream
    void encode(Stream&) const;

    // -- Accessors

    /// @returns size (rows * cols)
    inline Size size() const { return rows_*cols_; }
    /// @returns number of rows
    Size rows() const { return rows_; }
    /// @returns number of columns
    Size cols() const { return cols_; }

    /// Access by row and column
    /// @note implements column-major (Fortran-style) ordering
    Scalar& operator()(Size row, Size col) { return m_[col*rows_ + row]; }
    const Scalar& operator()(Size row, Size col) const { return m_[col*rows_ + row]; }

    /// Access to linearised storage
    Scalar& operator[](Size i) { return m_[i]; }
    const Scalar& operator[](Size i) const { return m_[i]; }

    /// @returns modifiable view of the data
    Scalar* data() { return m_; }
    /// @returns read-only view of the data
    const Scalar* data() const { return m_; }

    /// @returns iterator to beginning of the data
    Scalar* begin() { return m_; }
    /// @returns const iterator to beginning of the data
    const Scalar* begin() const { return m_; }
    /// @returns iterator to end of the data
    Scalar* end() { return m_ + size(); }
    /// @returns const iterator to end of the data
    const Scalar* end() const { return m_ + size(); }

protected:  // member variables

    /// Container
    Scalar* m_;

    /// Number of rows
    Size rows_;

    /// Number of columns
    Size cols_;

    /// Indicate ownership
    bool own_;

};

//-----------------------------------------------------------------------------

Stream& operator<<(Stream&, const Matrix&);

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif
