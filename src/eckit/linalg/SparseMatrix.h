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
#include "eckit/exception/Exceptions.h"

namespace eckit {

class Stream;
class PathName;

namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Sparse matrix in CRS (compressed row storage) format
///
class SparseMatrix {

public:  // methods

    // -- Constructors

    /// Default constructor, empty matrix
    SparseMatrix();

    /// Constructs an identity matrix with provided dimensions
    SparseMatrix(Size rows, Size cols);

    /// Constructor from triplets
    SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets);

    /// Construct vector from existing data (does NOT take ownership)
    SparseMatrix(Scalar* values, Size size, Size rows, Size cols, Index* outer, Index* inner);

    /// Constructor from Stream
    SparseMatrix(Stream& v);

    ~SparseMatrix();

    /// Copy constructor
    SparseMatrix(const SparseMatrix&);

    /// Assignment operator (allocates and copies data)
    SparseMatrix& operator=(const SparseMatrix&);

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

    /// @returns number of non-zeros
    Size nonZeros() const { return size_; }

    /// @returns true if this matrix does not contain non-zero entries
    bool empty() const { return !nonZeros(); }

    /// @returns read-only view of the data vector
    const Scalar* data() const { return data_; }
    /// @returns read-only view of the outer index vector
    const Index* outer() const { return outer_; }
    /// @returns read-only view of the inner index vector
    const Index* inner() const { return inner_; }

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    void reserve(Size rows, Size cols, Size nnz);

    /// Returns the footprint of the matrix in memory
    size_t footprint() const;

public: // iterators

    struct const_iterator {
        const_iterator(const SparseMatrix& matrix, Size row = 0) :
            matrix_(const_cast< SparseMatrix& >(matrix)),
            index_(0)
        {
            positionToRow(row);
        }

        Size col() const { return Size(matrix_.inner_[index_]); }
        Size row() const { NOTIMP; }

        operator bool() const { return index_ < matrix_.size_; }
        const_iterator& operator++();
        const_iterator operator++(int);

        bool operator==(const const_iterator& other) const {
            return     &other.matrix_ == &matrix_
                    && other.index_ == index_;
        }

        bool operator!=(const const_iterator& other) const { return operator==(other); }

        const Scalar& value() const { return matrix_.data_[index_]; }
        const Scalar& operator*() const;

    protected:

        void positionToRow(Size row);

        SparseMatrix& matrix_;
        Size index_;
    };

    struct iterator : const_iterator {
        iterator(SparseMatrix& matrix) : const_iterator(matrix) {}
        Scalar& value() { return matrix_.data_[index_]; }
        Scalar& operator*();
    };

    const_iterator row(const Size& row) const;
    iterator row(const Size& row);

private: // methods

    /// Resets the matrix to a deallocated state
    void reset();

    /// data size is the number of non-zeros
    Size dataSize() const { return nonZeros(); }

    /// inner size is the number of non-zeros
    Size innerSize() const { return nonZeros(); }

    /// @returns outer size is number of rows + 1
    Size outerSize() const { return Size(rows_ + 1); }

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

    Size         rows_;   ///< Number of rows
    Size         cols_;   ///< Number of columns

    bool own_;   ///< do we own the memory allocated in the containers ?

    friend Stream& operator<<(Stream&, const SparseMatrix&);
};

Stream& operator<<(Stream&, const SparseMatrix&);

//----------------------------------------------------------------------------------------------------------------------

} // namespace linalg
} // namespace eckit

#endif
