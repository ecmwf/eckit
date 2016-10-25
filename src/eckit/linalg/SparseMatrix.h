/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   SparseMatrix.h
/// @author Florian Rathgeber
/// @author Tiago Quintino
/// @date   June 2015

#ifndef eckit_la_SparseMatrix_h
#define eckit_la_SparseMatrix_h

#include <vector>

#include "eckit/linalg/types.h"
#include "eckit/linalg/Triplet.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

class Stream;
class PathName;

namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Sparse matrix in CRS (compressed row storage) format
///
class SparseMatrix : public eckit::NonCopyable {

public:  // methods

    // -- Constructors

    /// Default constructor, empty matrix
    SparseMatrix();

    /// Constructor an identity matrix with provided dimensions
    SparseMatrix(Size rows, Size cols);

    /// Constructor from triplets
    SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets);

    /// Construct vector from existing data (does NOT take ownership)
    SparseMatrix(Scalar* values, Size size, Index rows, Index cols, Index* outer, Index* inner);

    /// Constructor from Stream
    SparseMatrix(Stream& v);

    ~SparseMatrix();

public:

    /// Prune entries with exactly the given value
    SparseMatrix& prune(Scalar val = Scalar(0));

    /// Set matrix to the identity
    SparseMatrix& setIdentity(Size rows, Size cols);

    /// Transpose matrix in-place
    SparseMatrix& transpose();

    // I/O

    void save(const eckit::PathName &path) const;
    void load(const eckit::PathName &path);

    void swap(SparseMatrix& other);

    /// @returns number of rows
    Size rows() const { return rows_; }

    /// @returns number of columns
    Size cols() const { return cols_; }

    // Below only used by LinearAlgebraEigen and LinearAlgebraGeneric
    // maybe make them private and make these classes friends

    /// @returns number of non-zeros
    Size nonZeros() const { return size_; }

    /// @returns read-only view of the data vector
    const Scalar* data() const { return data_; }
    /// @returns read-only view of the outer index vector
    const Index* outer() const { return outer_; }
    /// @returns read-only view of the inner index vector
    const Index* inner() const { return inner_; }

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    void reserve(Index rows, Index cols, Size nnz);

    /// Returns the footprint of the matrix in memory
    size_t footprint() const;

private: // iterators

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

public: // iterators

    /// @todo Rename Vector iterators so they look like STL iterators

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

private: // methods

    /// Resets the matrix to a deallocated state
    void reset();

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

private: // members

    Scalar*      data_;   ///< matrix entries, sized with number of non-zeros (nnz)

    Size         size_;   ///< Size of the container (AKA number of non-zeros nnz)

    Index*       outer_;  ///< Starts of rows
    Index*       inner_;  ///< Column indices

    Index        rows_;   ///< Number of rows
    Index        cols_;   ///< Number of columns

    bool own_;   ///< do we own the memory allocated in the containers ?

    friend Stream& operator<<(Stream&, const SparseMatrix&);
};

Stream& operator<<(Stream&, const SparseMatrix&);

//----------------------------------------------------------------------------------------------------------------------

} // namespace linalg
} // namespace eckit

#endif
