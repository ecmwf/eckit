/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <numeric>

#include "eckit/exception/Exceptions.h"
#include "experimental/eckit/la/SparseMatrix.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix() {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols)
  : rows_(rows), cols_(cols) {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols, SparseMatrix::Size nnz)
  : v_(nnz), outer_(rows+1), inner_(nnz), rows_(rows), cols_(cols) {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols,
                           const SparseMatrix::ScalarStorage& v,
                           const SparseMatrix::IndexStorage& outer,
                           const SparseMatrix::IndexStorage& inner)
  : v_(v), rows_(rows), cols_(cols) {
    ASSERT(v.size() == inner.size() && outer.size() == rows+1);
}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(Stream& s) {
    s >> rows_;
    s >> cols_;
    Index nnz;
    s >> nnz;
    reserve(nnz);
    for (Index i = 0; i < rows_+1; ++i) s >> outer_[i];
    for (Index i = 0; i < nnz; ++i) s >> inner_[i];
    for (Index i = 0; i < nnz; ++i) s >> v_[i];
}

//-----------------------------------------------------------------------------

void SparseMatrix::resize(SparseMatrix::Size rows, SparseMatrix::Size cols) {
    v_.clear();
    outer_.clear();
    inner_.clear();
    rows_ = rows;
    cols_ = cols;
}

//-----------------------------------------------------------------------------

void SparseMatrix::reserve(SparseMatrix::Size nnz) {
    // Rows and columns must have been set before
    ASSERT( rows_ > 0 && cols_ > 0 );
    outer_.resize(rows_+1);
    inner_.resize(nnz);
    v_.resize(nnz);
}

//-----------------------------------------------------------------------------

void SparseMatrix::swap(SparseMatrix& other) {
    std::swap(v_, other.v_);
    std::swap(outer_, other.outer_);
    std::swap(inner_, other.inner_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
}

//-----------------------------------------------------------------------------

void SparseMatrix::setFromTriplets(const std::vector<Triplet>& triplets) {
    // Allocate memory (we are promised that there is 1 triplet per non-zero)
    reserve(triplets.size());

    Index pos = 0;
    Index row = -1;
    // Build vectors of inner indices and values, update outer index per row
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it, ++pos) {
        // We are promised triplets are ordered by rows
        ASSERT( it->row() >= row && it->row() < rows_ && it->col() >= 0 && it->col() < cols_ );
        // We start a new row
        while (it->row() > row)
            outer_[++row] = pos;
        inner_[pos] = it->col();
        v_[pos] = it->value();
    }
    outer_[rows_] = pos;
}

//-----------------------------------------------------------------------------

void SparseMatrix::assembleFromTriplets(const std::vector<Triplet>& triplets) {
    // Rows and columns must have been set before
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

//-----------------------------------------------------------------------------

void SparseMatrix::setIdentity() {
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

//-----------------------------------------------------------------------------

void SparseMatrix::prune(SparseMatrix::Scalar val) {
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

//-----------------------------------------------------------------------------

void SparseMatrix::encode(Stream& s) const {
    const Index nnz = outer_[rows_];
    s << rows_;
    s << cols_;
    s << nnz;
    for (Index i = 0; i < rows_+1; ++i) s << outer_[i];
    for (Index i = 0; i < nnz; ++i) s << inner_[i];
    for (Index i = 0; i < nnz; ++i) s << v_[i];
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& s, const SparseMatrix& v) {
    v.encode(s);
    return s;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
