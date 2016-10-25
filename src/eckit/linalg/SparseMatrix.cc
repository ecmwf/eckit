/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/linalg/SparseMatrix.h"

namespace eckit {
namespace linalg {

#ifdef EC_LITTLE_ENDIAN
static const bool littleEndian = true;
#else
static const bool littleEndian = false;
#endif

//----------------------------------------------------------------------------------------------------------------------

SparseMatrix::SparseMatrix() :
    data_(0),
    size_(0),
    outer_(0),
    inner_(0),
    rows_(0),
    cols_(0),
    own_(false) {
}

SparseMatrix::SparseMatrix(Size rows, Size cols) {
    zero(*this);
    reserve(rows, cols, 1);
}

SparseMatrix::SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets) {

    zero(*this);

    reserve(rows, cols, triplets.size()); // Allocate memory 1 triplet per non-zero

    Size pos = 0;
    Index row = -1;

    // Build vectors of inner indices and values, update outer index per row
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it, ++pos) {

        // triplets are ordered by rows
        ASSERT( it->row() >= row );
        ASSERT( it->row() < rows_ );
        ASSERT( it->col() >= 0 );
        ASSERT( it->col() < cols_ );

        // start a new row
        while (it->row() > row) {
            outer_[++row] = Index(pos);
        }

        inner_[pos] = it->col();
        data_[pos] = it->value();
    }

    while (row < rows_) {
        outer_[++row] = Index(pos);
    }
}


SparseMatrix::SparseMatrix(Stream &s) {
    zero(*this);
    decode(s);
}

SparseMatrix::SparseMatrix(Scalar* values, Size size, Index rows, Index cols, Index* outer, Index* inner)  :
    data_(values),
    size_(size),
    outer_(outer),
    inner_(inner),
    rows_(rows),
    cols_(cols),
    own_(false) {
}

SparseMatrix::~SparseMatrix() {
    reset();
}

SparseMatrix::SparseMatrix(const SparseMatrix& other) {

    zero(*this);

    if(!other.empty()) { // in case we copy an other that was constructed empty

        reserve(other.rows(), other.cols(), other.nonZeros());

        ::memcpy(data_,  other.data_,  dataSize()  * sizeof(Scalar));
        ::memcpy(outer_, other.outer_, outerSize() * sizeof(Index));
        ::memcpy(inner_, other.inner_, innerSize() * sizeof(Index));
    }
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other)
{
    SparseMatrix copy(other);
    swap(copy);
    return *this;
}

void SparseMatrix::reset() {

    if(own_) {
        delete [] inner_;
        delete [] outer_;
        delete [] data_;
    }

    own_  = false;
    rows_ = 0;
    cols_ = 0;
    size_ = 0;

    data_ = 0;
    outer_ = 0;
    inner_ = 0;
}

// variables into this method must be by value
void SparseMatrix::reserve(Index rows, Index cols, Size nnz) {

    ASSERT( nnz );
    ASSERT( nnz <= Size(rows * cols) );
    ASSERT( rows > 0 && cols > 0 ); /* rows and columns must have some size */

    reset();

    rows_ = rows;
    cols_ = cols;
    size_ = nnz;

    data_  = new Scalar[dataSize()];
    outer_ = new Index[outerSize()];
    inner_ = new Index[innerSize()];

    own_  = true;
}



void SparseMatrix::swap(SparseMatrix &other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(outer_, other.outer_);
    std::swap(inner_, other.inner_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(own_, other.own_);
}

size_t SparseMatrix::footprint() const {
    return sizeof(*this)
            + dataSize()  * sizeof(Scalar)
            + innerSize() * sizeof(Index)
            + outerSize() * sizeof(Index);
}


SparseMatrix& SparseMatrix::setIdentity(Size rows, Size cols) {

    ASSERT( rows > 0 && cols > 0 );

    rows_ = rows;
    cols_ = cols;

    Index nnz = std::min(rows_, cols_);

    reserve(rows_, cols_, nnz);

    for(Index i = 0; i < nnz; ++i) {
        outer_[i] = i;
        inner_[i] = i;
    }

    for(Index i = nnz; i <= rows_; ++i) {
        outer_[i] = nnz;
    }

    for(Size i = 0; i < size_; ++i) {
        data_[i] = Scalar(1);
    }

    return *this;
}



SparseMatrix& SparseMatrix::transpose() {

    /// @note Can SparseMatrix::transpose() be done more efficiently?
    ///       We are building another matrix and then swapping

    std::vector<Triplet> triplets;
    triplets.reserve(nonZeros());
    for (Index r = 0; r < rows_; ++r)
        for (Index c = outer_[r]; c < outer_[r + 1]; ++c)
            triplets.push_back(Triplet(inner_[c], r, data_[c]));

    std::sort(triplets.begin(), triplets.end()); // triplets must be sorted by row

    SparseMatrix tmp(cols_, rows_, triplets);

    swap(tmp);

    return *this;
}

SparseMatrix& SparseMatrix::prune(linalg::Scalar val) {

    std::vector<Scalar> v;
    std::vector<Index> inner;

    Index nnz = 0;
    for(Index r = 0; r < rows_; ++r) {
        const Index start = outer_[r];
        outer_[r] = nnz;
        for(Index c = start; c < outer_[r + 1]; ++c)
            if(data_[c] != val) {
                v.push_back(data_[c]);
                inner.push_back(inner_[c]);
                ++nnz;
            }
    }
    outer_[rows_] = nnz;

    SparseMatrix tmp;
    tmp.reserve(rows_, cols_, nnz);

    ::memcpy(tmp.data_,  v.data(),  nnz * sizeof(Scalar));
    ::memcpy(tmp.outer_, outer_, outerSize() * sizeof(Index));
    ::memcpy(tmp.inner_, inner.data(), nnz * sizeof(Index));

    swap(tmp);

    return *this;
}



void SparseMatrix::encode(Stream &s) const {

    s << rows_;
    s << cols_;
    s << size_;

    s << littleEndian;
    s << sizeof(Index);
    s << sizeof(Scalar);
    s << sizeof(Size);

    s.writeLargeBlob(outer_, outerSize() * sizeof(Index));
    s.writeLargeBlob(inner_, innerSize() * sizeof(Index));
    s.writeLargeBlob(data_,  dataSize()  * sizeof(Scalar));
}

void SparseMatrix::decode(Stream &s) {

    Index rows;
    Index cols;
    Size nnz;

    s >> rows;
    s >> cols;
    s >> nnz;

    bool little_endian;
    s >> little_endian; ASSERT(littleEndian == little_endian);

    size_t index_size;
    s >> index_size; ASSERT(index_size == sizeof(Index));

    size_t scalar_size;
    s >> scalar_size; ASSERT(scalar_size == sizeof(Scalar));

    size_t size_size;
    s >> size_size; ASSERT(size_size == sizeof(Size));

    reserve(rows, cols, nnz);

    s.readLargeBlob(outer_, outerSize() * sizeof(Index));
    s.readLargeBlob(inner_, innerSize() * sizeof(Index));
    s.readLargeBlob(data_,  dataSize()  * sizeof(Scalar));
}



Stream &operator<<(Stream &s, const SparseMatrix &v) {
    v.encode(s);
    return s;
}

SparseMatrix::_InnerIterator::_InnerIterator(SparseMatrix &m, Index outer)
    : matrix_(m), outer_(outer), inner_(matrix_.outer_[outer]) {
    ASSERT(outer >= 0);
    ASSERT(outer < m.rows_);
    ASSERT(inner_ >= 0);
    ASSERT(size_t(inner_) <= matrix_.nonZeros());
}

Scalar SparseMatrix::_InnerIterator::operator*() const {
    ASSERT(size_t(inner_) < matrix_.nonZeros());
    return matrix_.data_[inner_];
}

Scalar &SparseMatrix::InnerIterator::operator*() {
    ASSERT(size_t(inner_) < matrix_.nonZeros());
    return matrix_.data_[inner_];
}



void SparseMatrix::save(const eckit::PathName &path) const {
    FileStream s(path, "w");
    s << *this;
}

void SparseMatrix::load(const eckit::PathName &path)  {
    FileStream s(path, "r");
    decode(s);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit
