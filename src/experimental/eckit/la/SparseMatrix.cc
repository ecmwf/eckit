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

// For endianness
#include "eckit/eckit.h"


#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"

#include "experimental/eckit/la/SparseMatrix.h"

namespace eckit {
namespace la {

#ifdef EC_LITTLE_ENDIAN
static const bool littleEndian = true;
#else
static const bool littleEndian = false;
#endif

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix() {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols)
    : rows_(rows), cols_(cols) {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(Stream &s) {
    decode(s);
}

//-----------------------------------------------------------------------------

void SparseMatrix::resize(SparseMatrix::Size rows, SparseMatrix::Size cols) {
    data_.clear();
    outer_.clear();
    inner_.clear();
    rows_ = rows;
    cols_ = cols;
}

//-----------------------------------------------------------------------------

void SparseMatrix::reserve(SparseMatrix::Size nnz) {
    // Rows and columns must have been set before
    ASSERT( rows_ > 0 && cols_ > 0 );
    outer_.resize(rows_ + 1);
    inner_.resize(nnz);
    data_.resize(nnz);
}

//-----------------------------------------------------------------------------

void SparseMatrix::swap(SparseMatrix &other) {
    std::swap(data_, other.data_);
    std::swap(outer_, other.outer_);
    std::swap(inner_, other.inner_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
}

//-----------------------------------------------------------------------------

SparseMatrix& SparseMatrix::setFromTriplets(const std::vector<Triplet> &triplets) {
    // Allocate memory (we are promised that there is 1 triplet per non-zero)
    reserve(triplets.size());

    Size pos = 0;
    Index row = -1;
    // Build vectors of inner indices and values, update outer index per row
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it, ++pos) {
        // We are promised triplets are ordered by rows
        ASSERT( it->row() >= row );
        ASSERT( it->row() < rows_ );
        ASSERT( it->col() >= 0 );
        ASSERT( it->col() < cols_ );
        // We start a new row
        while (it->row() > row) {
            outer_[++row] = Index(pos);
        }
        inner_[pos] = it->col();
        data_[pos] = it->value();
    }

    while (row < rows_) {
        outer_[++row] = Index(pos);
    }
    return *this;
}

//-----------------------------------------------------------------------------

SparseMatrix& SparseMatrix::setIdentity() {
    Index nnz = std::min(rows_, cols_);
    outer_.resize(rows_ + 1);
    inner_.resize(nnz);
    for (Index i = 0; i < nnz; ++i) {
        outer_[i] = i;
        inner_[i] = i;
    }
    for (Index i = nnz; i <= rows_; ++i) {
        outer_[i] = nnz;
    }
    data_.assign(nnz, Scalar(1));
    return *this;
}

//-----------------------------------------------------------------------------

SparseMatrix& SparseMatrix::transpose() {
    // FIXME: can this be done more efficiently?
    // Create vector of transposed triplets
    std::vector<Triplet> triplets;
    triplets.reserve(nonZeros());
    for (Index r = 0; r < rows_; ++r)
        for (Index c = outer_[r]; c < outer_[r + 1]; ++c)
            triplets.push_back(Triplet(inner_[c], r, data_[c]));
    // Need to sort since setFromTriplets expects triplets ordered by row
    std::sort(triplets.begin(), triplets.end());
    std::swap(rows_, cols_);
    return setFromTriplets(triplets);
}

//-----------------------------------------------------------------------------

SparseMatrix& SparseMatrix::prune(la::Scalar val) {
    ScalarStorage v;
    IndexStorage inner;
    Index nnz = 0;
    for (Index r = 0; r < rows_; ++r) {
        const Index start = outer_[r];
        outer_[r] = nnz;
        for (Index c = start; c < outer_[r + 1]; ++c)
            if (data_[c] != val) {
                v.push_back(data_[c]);
                inner.push_back(inner_[c]);
                ++nnz;
            }
    }
    outer_[rows_] = nnz;
    std::swap(v, data_);
    std::swap(inner, inner_);
    return *this;
}

//-----------------------------------------------------------------------------

void SparseMatrix::encode(Stream &s) const {
    const Index nnz = outer_[rows_];
    s << rows_;
    s << cols_;
    s << nnz;

    s << littleEndian;
    s << sizeof(Index);
    s << sizeof(Scalar);

    s.writeLargeBlob(&outer_[0], outer_.size() * sizeof(Index));
    s.writeLargeBlob(&inner_[0], inner_.size() * sizeof(Index));
    s.writeLargeBlob(&data_[0], data_.size() * sizeof(Scalar));
}

void SparseMatrix::decode(Stream &s) {

    s >> rows_;
    s >> cols_;
    Index nnz;
    s >> nnz;
    reserve(nnz);

    bool little_endian;
    s >> little_endian; ASSERT(littleEndian == little_endian);

    size_t index_size;
    s >> index_size; ASSERT(index_size == sizeof(Index));

    size_t scalar_size;
    s >> scalar_size; ASSERT(scalar_size == sizeof(Scalar));

    s.readLargeBlob(&outer_[0], outer_.size() * sizeof(Index));
    s.readLargeBlob(&inner_[0], inner_.size() * sizeof(Index));
    s.readLargeBlob(&data_[0], data_.size() * sizeof(Scalar));

}

//-----------------------------------------------------------------------------

Stream &operator<<(Stream &s, const SparseMatrix &v) {
    v.encode(s);
    return s;
}

SparseMatrix::_InnerIterator::_InnerIterator(SparseMatrix &m, SparseMatrix::Index outer)
    : matrix_(m), outer_(outer), inner_(matrix_.outer_[outer]) {
    ASSERT(outer >= 0);
    ASSERT(outer < m.rows_);
    ASSERT(inner_ >= 0);
    ASSERT(size_t(inner_) <= matrix_.data_.size());
}

SparseMatrix::Scalar SparseMatrix::_InnerIterator::operator*() const {
    ASSERT(size_t(inner_) < matrix_.data_.size());
    return matrix_.data_[inner_];
}

SparseMatrix::Scalar &SparseMatrix::InnerIterator::operator*() {
    ASSERT(size_t(inner_) < matrix_.data_.size());
    return matrix_.data_[inner_];
}

//-----------------------------------------------------------------------------

void SparseMatrix::save(const eckit::PathName &path) const {
    FileStream s(path, "w");
    s << *this;
}

void SparseMatrix::load(const eckit::PathName &path)  {
    FileStream s(path, "r");
    decode(s);
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
