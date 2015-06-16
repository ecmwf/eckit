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
    typedef Storage::size_type Index;

public:  // methods

    // -- Constructors

    /// Default constructor (empty matrix)
    Matrix() {}

    /// Construct matrix with given rows and columns (allocates memory)
    Matrix(Index rows, Index cols) : v_(rows*cols), rows_(rows), cols_(cols) {}

    /// @returns size (rows * cols)
    Index size() const { return v_.size(); }
    /// @returns number of rows
    Index rows() const { return rows_; }
    /// @returns number of columns
    Index cols() const { return cols_; }

    Scalar& operator()(Index row, Index col) { return v_[col*rows_ + row]; }
    const Scalar& operator()(Index row, Index col) const { return v_[col*rows_ + row]; }

    /// @returns modifiable view of the data
    Scalar* data() { return v_.data(); }
    /// @returns read-only view of the data
    const Scalar* data() const { return v_.data(); }

protected:  // members
    Storage v_;
    Index rows_;
    Index cols_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
