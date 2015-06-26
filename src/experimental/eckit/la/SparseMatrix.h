/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   SparseMatrix.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_SparseMatrix_h
#define eckit_la_SparseMatrix_h

#include <vector>

#include "experimental/eckit/la/types.h"
#include "experimental/eckit/la/Triplet.h"

namespace eckit {

class Stream;

namespace la {

//-----------------------------------------------------------------------------

/// Sparse matrix in CRS (compressed row storage) format
class SparseMatrix {
public:  // types
    typedef la::Index Index;
    typedef la::Scalar Scalar;
    typedef std::vector<Scalar> ScalarStorage;
    typedef std::vector<Index> IndexStorage;
    typedef ScalarStorage::size_type Size;

public:  // methods

    // -- Constructors

    /// Default constructor, empty matrix
    SparseMatrix();

    /// Constructor setting sizes, empty matrix
    SparseMatrix(Size rows, Size cols);

    /// Constructor setting sizes and number of non-zeros, allocates memory
    SparseMatrix(Size rows, Size cols, Size nnz);

    /// Constructor initialising matrix with existing storage
    SparseMatrix(Size rows, Size cols,
                 const ScalarStorage& v, const IndexStorage& outer, const IndexStorage& inner);

    /// Constructor from Stream
    SparseMatrix(Stream& v);

    // -- Mutators

    /// Resize sparse matrix (invalidates all data arrays)
    void resize(Size rows, Size cols);

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    void reserve(Size nnz);

    /// Swap sparse matrix with another
    void swap(SparseMatrix& other);

    /// Initialise matrix from vector of triplets
    ///
    /// Resets data and index vectors, requires row and column number to be set.
    /// Assumes triplets are ordered per row and there are no duplicates.
    void setFromTriplets(const std::vector<Triplet>& triplets);

    /// Initialise matrix from vector of triplets
    ///
    /// Resets data and index vectors, requires row and column number to be set.
    /// Assumes no order of triplets and allows duplicates, which are summed.
    void assembleFromTriplets(const std::vector<Triplet>& triplets);

    /// Set matrix to the identity
    void setIdentity();

    /// Prune entries with exactly the given value
    void prune(Scalar val = Scalar(0));

    // -- Serialisation

    /// Serialise to a Stream
    void encode(Stream& s) const;

    /// Eigen-compatible iterator over nonzeros of a given row
    class InnerIterator {
    public:
        InnerIterator(SparseMatrix& m, Index outer);
        Scalar& value() { return m_.v_[inner_]; }
        Scalar value() const { return m_.v_[inner_]; }
        Index row() const { return outer_; }
        Index col() const { return m_.inner_[inner_]; }
        Index index() const { return m_.inner_[inner_]; }
        operator bool() const { return inner_ != m_.outer_[outer_+1]; }
        void operator++() { ++inner_; }
        Scalar& operator*();
        Scalar operator*() const;
    private:
        SparseMatrix& m_;
        Index outer_;
        Index inner_;
    };

    // -- Accessors

    /// @returns size (rows * cols)
    Size size() const { return rows_*cols_; }
    /// @returns number of rows
    Size rows() const { return rows_; }
    /// @returns number of columns
    Size cols() const { return cols_; }
    /// @returns inner size (number of columns)
    Size innerSize() const { return cols_; }
    /// @returns outer size (number of rows)
    Size outerSize() const { return rows_; }
    /// @returns number of non-zeros
    Size nonZeros() const { return v_.size(); }

    /// @returns modifiable view of the data vector
    Scalar* data() { return v_.data(); }
    /// @returns read-only view of the data vector
    const Scalar* data() const { return v_.data(); }
    /// @returns modifiable view of the outer index vector
    Index* outer() { return outer_.data(); }
    /// @returns read-only view of the outer index vector
    const Index* outer() const { return outer_.data(); }
    /// @returns modifiable view of the inner index vector
    Index* inner() { return inner_.data(); }
    /// @returns read-only view of the inner index vector
    const Index* inner() const { return inner_.data(); }

protected:  // members
    ScalarStorage v_;     /// Data
    IndexStorage outer_;  /// Starts of rows
    IndexStorage inner_;  /// Column indices
    Index rows_;          /// Number of rows
    Index cols_;          /// Number of columns
};

//-----------------------------------------------------------------------------

Stream& operator<<(Stream&, const SparseMatrix&);

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
