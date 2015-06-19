/*
 * (C) Copyright 1996-2015 ECMWF.
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

#ifndef eckit_la_Matrix_h
#define eckit_la_Matrix_h

#include <vector>

#include "eckit/la/types.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

/// Dense matrix in column major storage order
class Matrix {
public:  // types
    typedef std::vector<Scalar> Storage;
    typedef Storage::size_type Size;

public:  // methods

    // -- Constructors

    /// Default constructor (empty matrix)
    Matrix() {}

    /// Construct matrix with given rows and columns (allocates memory)
    Matrix(Size rows, Size cols) : v_(rows*cols), rows_(rows), cols_(cols) {}

    /// Resize matrix to given number of rows/columns (invalidates data)
    void resize(Size rows, Size cols) {
        v_.resize(rows*cols);
        rows_ = rows;
        cols_ = cols;
    }

    /// Swap this matrix with another
    void swap(Matrix& o) {
        v_.swap(o.v_);
        std::swap(rows_, o.rows_);
        std::swap(cols_, o.cols_);
    }

    /// @returns size (rows * cols)
    Size size() const { return v_.size(); }
    /// @returns number of rows
    Size rows() const { return rows_; }
    /// @returns number of columns
    Size cols() const { return cols_; }

    Scalar& operator()(Size row, Size col) { return v_[col*rows_ + row]; }
    const Scalar& operator()(Size row, Size col) const { return v_[col*rows_ + row]; }

    /// @returns modifiable view of the data
    Scalar* data() { return v_.data(); }
    /// @returns read-only view of the data
    const Scalar* data() const { return v_.data(); }

    /// @returns iterator to beginning of the data
    Storage::iterator begin() { return v_.begin(); }
    /// @returns const iterator to beginning of the data
    Storage::const_iterator begin() const { return v_.begin(); }
    /// @returns iterator to end of the data
    Storage::iterator end() { return v_.end(); }
    /// @returns const iterator to end of the data
    Storage::const_iterator end() const { return v_.end(); }

protected:  // members
    Storage v_;
    Size rows_;
    Size cols_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
