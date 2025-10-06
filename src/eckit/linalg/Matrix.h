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

#include "eckit/linalg/types.h"


namespace eckit {
class Stream;
}

namespace eckit::linalg {

//-----------------------------------------------------------------------------

/// Dense matrix in column major storage order
class Matrix {
public:  // types

    using Size = linalg::Size;

public:  // methods

    // -- Constructors

    /// Default constructor (empty matrix)
    Matrix();

    /// Construct matrix with given rows and columns (allocates memory, not initialised)
    Matrix(Size rows, Size cols);

    /// Construct matrix from existing data (does NOT take ownership)
    Matrix(const Scalar* array, Size rows, Size cols);

    /// Constructor from Stream
    Matrix(Stream&);

    /// Copy constructor
    Matrix(const Matrix&);

    /// Destructor
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
    Size size() const { return rows_ * cols_; }

    /// @returns number of rows
    Size rows() const { return rows_; }

    /// @returns number of columns
    Size cols() const { return cols_; }

    /// Access by row and column
    /// @note implements column-major (Fortran-style) ordering
    Scalar& operator()(Size row, Size col) { return array_[col * rows_ + row]; }
    const Scalar& operator()(Size row, Size col) const { return array_[col * rows_ + row]; }

    /// Access to linearised storage
    Scalar& operator[](Size i) { return array_[i]; }
    const Scalar& operator[](Size i) const { return array_[i]; }

    /// @returns modifiable view of the data
    Scalar* data() { return array_; }
    /// @returns read-only view of the data
    const Scalar* data() const { return array_; }

    /// @returns iterator to beginning of the data
    Scalar* begin() { return array_; }

    /// @returns const iterator to beginning of the data
    const Scalar* begin() const { return array_; }

    /// @returns iterator to end of the data
    Scalar* end() { return array_ + size(); }

    /// @returns const iterator to end of the data
    const Scalar* end() const { return array_ + size(); }

protected:  // member variables

    /// Container
    Scalar* array_;

    /// Number of rows
    Size rows_;

    /// Number of columns
    Size cols_;

    /// Indicate ownership
    bool own_;
};


Stream& operator<<(Stream&, const Matrix&);


//-----------------------------------------------------------------------------

}  // namespace eckit::linalg
