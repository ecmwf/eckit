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

#include <numeric>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/la/types.h"
#include "eckit/la/Triplet.h"

namespace eckit {
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
    SparseMatrix() {}

    /// Constructor setting sizes, empty matrix
    SparseMatrix(Size rows, Size cols) : rows_(rows), cols_(cols) {}

    /// Constructor setting sizes and number of non-zeros, allocates memory
    SparseMatrix(Size rows, Size cols, Size nnz)
        : v_(nnz), outer_(rows+1), inner_(nnz), rows_(rows), cols_(cols) {}

    /// Constructor initialising matrix with existing storage
    SparseMatrix(Size rows, Size cols,
                 const ScalarStorage& v, const IndexStorage& outer, const IndexStorage& inner)
        : v_(v), rows_(rows), cols_(cols) {
        ASSERT(v.size() == inner.size() && outer.size() == rows+1);
    }

    /// Resize sparse matrix (invalidates all data arrays)
    void resize(Size rows, Size cols) {
        v_.clear();
        outer_.clear();
        inner_.clear();
        rows_ = rows;
        cols_ = cols;
    }

    /// Initialise matrix from vector of triplets
    ///
    /// Resets data and index vectors, requires row and column number to be set.
    /// Assumes no order of triplets and allows duplicates, which are summed.
    void setFromTriplets(const std::vector<Triplet>& triplets) {
        ASSERT( rows_ > 0 && cols_ > 0 );

        // Build sparsity pattern i.e. inner indices for each row
        std::vector<std::set<Index> > nz(rows_);
        for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it) {
            ASSERT(it->row() >= 0 && it->row() < rows_ && it->col() >= 0 && it->col() < cols_);
            nz[it->row()].insert(it->col());
        }

        // Build vector of outer indices
        outer_.resize(rows_+1);
        outer_[0] = 0;
        for (Index i = 0; i < rows_; ++i)
            outer_[i+1] = outer_[i] + nz[i].size();
        Size nnz = outer_[rows_];

        // Build vectors of inner indices and values
        inner_.resize(nnz);
        v_.assign(nnz, Scalar(0));
        for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it) {
            const Size row = it->row();
            const Size col = it->col();
            // Find the position in the current row
            const Size pos = outer_[row] + std::distance(nz[row].begin(), nz[row].find(col));
            inner_[pos] = col;
            v_[pos] += it->value();
        }
    }

    /// Set matrix to the identity
    void setIdentity() {
        Index nnz = std::min(rows_, cols_);
        outer_.resize(rows_+1);
        inner_.resize(nnz);
        for (Index i = 0; i < nnz; ++i) {
            outer_[i] = i;
            inner_[i] = i;
        }
        for (Index i = nnz; i <= rows_; ++i)
            outer_[i] = nnz;
        v_.assign(nnz, Scalar(1));
    }

    /// Prune entries with exactly the given value
    void prune(Scalar val = Scalar(0)) {
        ScalarStorage v;
        IndexStorage outer;
        IndexStorage inner;
        Index nnz = 0;
        for (Index r = 0; r < rows_; ++r) {
            outer.push_back(nnz);
            for (Index c = outer_[r]; c < outer_[r+1]; ++c)
                if (v_[c] != val) {
                    v.push_back(v_[c]);
                    inner.push_back(inner_[c]);
                    ++nnz;
                }
        }
        outer.push_back(nnz);
        std::swap(v, v_);
        std::swap(outer, outer_);
        std::swap(inner, inner_);
    }

    /// Eigen-compatible iterator over nonzeros of a given row
    class InnerIterator {
    public:
        InnerIterator(SparseMatrix& m, Index outer)
            : m_(m), outer_(outer), inner_(m_.outer_[outer]) {}
        Scalar& value() { return m_.v_[inner_]; }
        Scalar value() const { return m_.v_[inner_]; }
        Index row() const { return outer_; }
        Index col() const { return m_.inner_[inner_]; }
        Index index() const { return m_.inner_[inner_]; }
        operator bool() const { return inner_ != m_.outer_[outer_+1]; }
        void operator++() { ++inner_; }
        Scalar& operator*() { return m_.v_[inner_]; }
        Scalar operator*() const { return m_.v_[inner_]; }
    private:
        SparseMatrix& m_;
        Index outer_;
        Index inner_;
    };

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

}  // namespace la
}  // namespace eckit

#endif
