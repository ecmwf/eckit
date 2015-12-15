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

#include "eckit/la/types.h"
#include "eckit/la/Triplet.h"

namespace eckit {

class Stream;
class PathName;

namespace la {

//-----------------------------------------------------------------------------

/// Sparse matrix in CRS (compressed row storage) format
class SparseMatrix {
public:  // types
    typedef la::Index Index;
    typedef la::Scalar Scalar;
    // -- Types
private:
    typedef std::vector<Scalar> ScalarStorage;
    typedef std::vector<Index> IndexStorage;
public:
    typedef ScalarStorage::size_type Size;

public:  // methods

    // -- Constructors

    /// Default constructor, empty matrix
    SparseMatrix();

    /// Constructor setting sizes, empty matrix
    SparseMatrix(Size rows, Size cols);

    /// Constructor from Stream
    SparseMatrix(Stream& v);

public:

    /// Initialise matrix from vector of triplets
    ///
    /// Resets data and index vectors, requires row and column number to be set.
    /// Assumes triplets are ordered per row and there are no duplicates.
    SparseMatrix& setFromTriplets(const std::vector<Triplet>& triplets);
    /// Prune entries with exactly the given value
    SparseMatrix& prune(Scalar val = Scalar(0));
    /// Set matrix to the identity
    SparseMatrix& setIdentity();
    /// Transpose matrix in-place
    SparseMatrix& transpose();

    /// I/O

    void save(const eckit::PathName &path) const;
    void load(const eckit::PathName &path);

    //
    void swap(SparseMatrix& other);

    /// @returns number of rows
    Size rows() const { return rows_; }
    /// @returns number of columns
    Size cols() const { return cols_; }

    // Below only used by LinearAlgebraEigen and
    // LinearAlgebraGeneric
    // maybe make them private and make these classes friends

    /// @returns number of non-zeros
    Size nonZeros() const { return data_.size(); }
    /// @returns read-only view of the data vector
    const Scalar* data() const { return data_.data(); }
    /// @returns read-only view of the outer index vector
    const Index* outer() const { return outer_.data(); }
    /// @returns read-only view of the inner index vector
    const Index* inner() const { return inner_.data(); }

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    void reserve(Size nnz);

private:
// --- Iterator
    class _InnerIterator {
    public:
        _InnerIterator(SparseMatrix& m, Index outer);
        Scalar value() const { return matrix_.data_[inner_]; }
        Index row() const { return outer_; }
        Index col() const { return matrix_.inner_[inner_]; }
        Index index() const { return matrix_.inner_[inner_]; }
        operator bool() const { return inner_ != matrix_.outer_[outer_+1]; }
        void operator++() { ++inner_; }
        Scalar operator*() const;
    protected:
        SparseMatrix& matrix_;
        Index outer_;
        Index inner_;
    };
public:

    // TODO: Rename so they look like STL iterators

    class InnerIterator : public _InnerIterator {
    public:
        InnerIterator(SparseMatrix& matrix, Index outer):
            _InnerIterator(matrix, outer) {}
        Scalar& value() { return matrix_.data_[inner_]; }
        Scalar& operator*();
    };

    class ConstInnerIterator : public _InnerIterator  {
    public:
        ConstInnerIterator(const SparseMatrix& matrix, Index outer):
            _InnerIterator(const_cast<SparseMatrix&>(matrix), outer) {}
    };

private:


    // members

    ScalarStorage data_;     /// Data
    IndexStorage outer_;  /// Starts of rows
    IndexStorage inner_;  /// Column indices
    Index rows_;          /// Number of rows
    Index cols_;          /// Number of columns

    // methods

    /// @returns inner size (number of columns)
    Size innerSize() const { return cols_; }

    /// @returns outer size (number of rows)
    Size outerSize() const { return rows_; }

    /// Serialise to a Stream
    void encode(Stream& s) const;

    /// Deserialise from a Stream
    void decode(Stream& s);
    /// Resize sparse matrix (invalidates all data arrays)
    void resize(Size rows, Size cols);

// Friends:
    friend Stream& operator<<(Stream&, const SparseMatrix&);
    // friend class LinearAlgebraEigen;
    // friend class LinearAlgebraGeneric;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
