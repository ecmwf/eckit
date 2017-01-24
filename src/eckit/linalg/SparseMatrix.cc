/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/SparseMatrix.h"

#include <algorithm>
#include <iterator>
#include <numeric>

// For endianness
#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/io/MemoryHandle.h"


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
    Size row = 0;

    outer_[0] = 0; /* first entry is always zero */

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

        inner_[pos] = Index(it->col());
        data_[pos] = it->value();
    }

    while (row < rows_) {
        outer_[++row] = Index(pos);
    }

    ASSERT(Size(outer_[ outerSize() - 1 ]) == nonZeros()); /* last entry is always the nnz */
}


SparseMatrix::SparseMatrix(Stream &s) {
    zero(*this);
    decode(s);
}

SparseMatrix::SparseMatrix(Scalar* values, Size size, Size rows, Size cols, Index* outer, Index* inner)  :
    data_(values),
    size_(size),
    outer_(outer),
    inner_(inner),
    rows_(rows),
    cols_(cols),
    own_(false) {
}


struct SPMInfo {
    Size         size_;   ///< non-zeros
    Size         rows_;   ///< rows
    Size         cols_;   ///< columns
    ptrdiff_t    data_;
    ptrdiff_t    outer_;
    ptrdiff_t    inner_;
};

SparseMatrix::SparseMatrix(const eckit::Buffer& buffer)
{
    size_t buffsize = buffer.size();

    const char* b = buffer;
    char* addr = const_cast<char*>(b);

    eckit::MemoryHandle mh(buffer);
    mh.openForRead();

    struct SPMInfo info;
    mh.read(&info, sizeof(SPMInfo));

    ASSERT(info.size_ && info.rows_ && info.cols_);
    ASSERT(info.data_ > 0 && info.outer_ > 0 && info.inner_ > 0);

    own_  = false;
    size_ = info.size_;
    rows_ = info.rows_;
    cols_ = info.cols_;

    ASSERT(buffer.size() >= sizeof(SPMInfo) +
                           + sizeofData()
                           + sizeofOuter()
                           + sizeofInner() );

    data_  = reinterpret_cast<Scalar*>(addr + info.data_);
    outer_ = reinterpret_cast<Index*>(addr + info.outer_);
    inner_ = reinterpret_cast<Index*>(addr + info.inner_);

    // check offsets don't segfault

    ASSERT(info.data_  + sizeofData()  <= buffsize);
    ASSERT(info.outer_ + sizeofOuter() <= buffsize);
    ASSERT(info.inner_ + sizeofInner() <= buffsize);
}

SparseMatrix::~SparseMatrix() {
    reset();
}

SparseMatrix::SparseMatrix(const SparseMatrix& other) {

    zero(*this);

    if(!other.empty()) { // in case we copy an other that was constructed empty

        reserve(other.rows(), other.cols(), other.nonZeros());

        ::memcpy(data_,  other.data_,  sizeofData());
        ::memcpy(outer_, other.outer_, sizeofOuter());
        ::memcpy(inner_, other.inner_, sizeofInner());
    }
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other) {
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
void SparseMatrix::reserve(Size rows, Size cols, Size nnz) {

    ASSERT( nnz );
    ASSERT( nnz <= rows * cols );
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


void SparseMatrix::save(const eckit::PathName &path) const {
    FileStream s(path, "w");
    s << *this;
}


void SparseMatrix::load(const eckit::PathName &path)  {
    FileStream s(path, "r");
    decode(s);
}

void SparseMatrix::dump(Buffer& buffer) const {

    size_t minimum = sizeof(SPMInfo) + sizeofData() + sizeofOuter() + sizeofInner();
    ASSERT( buffer.size() >= minimum);

    MemoryHandle mh(buffer);
    mh.openForWrite(buffer.size());

    SPMInfo info;

    info.size_  = nonZeros();
    info.rows_  = rows();
    info.cols_  = cols();

    info.data_  = sizeof(SPMInfo);
    info.outer_ = info.data_  + sizeofData();
    info.inner_ = info.outer_ + sizeofOuter();

    /// @todo we should try to get these memory aligned (to say 64 bytes)

    mh.write(&info, sizeof(SPMInfo));

    ASSERT(mh.write(data_,  sizeofData())  == long(sizeofData()));
    ASSERT(mh.write(outer_, sizeofOuter()) == long(sizeofOuter()));
    ASSERT(mh.write(inner_, sizeofInner()) == long(sizeofInner()));
}

void SparseMatrix::swap(SparseMatrix &other) {
    std::swap(data_,  other.data_);
    std::swap(size_,  other.size_);
    std::swap(outer_, other.outer_);
    std::swap(inner_, other.inner_);
    std::swap(rows_,  other.rows_);
    std::swap(cols_,  other.cols_);
    std::swap(own_,   other.own_);
}


size_t SparseMatrix::footprint() const {
    return sizeof(*this)
            + sizeofData()
            + sizeofOuter()
            + sizeofInner();
}

void SparseMatrix::dump(std::ostream& os) const
{
    for(Size i = 0; i < rows(); ++i) {

        const_iterator itr = begin(i);
        const_iterator iend = end(i);

        if(itr == iend) continue;
        os << itr.row();

        for(; itr != iend; ++itr) {
            os << " " << itr.col() << " " << *itr;
        }
        os << std::endl;
    }
}

void SparseMatrix::print(std::ostream& os) const
{
    os << "SparseMatrix["
       << "nnz="  << size_ << ","
       << "rows=" << rows_ << ","
       << "cols=" << cols_ << "]";
}

SparseMatrix& SparseMatrix::setIdentity(Size rows, Size cols) {

    ASSERT( rows > 0 && cols > 0 );

    rows_ = rows;
    cols_ = cols;

    Size nnz = std::min(rows_, cols_);

    reserve(rows_, cols_, nnz);

    for(Size i = 0; i < nnz; ++i) {
        outer_[i] = Index(i);
        inner_[i] = Index(i);
    }

    for(Size i = nnz; i <= rows_; ++i) {
        outer_[i] = Index(nnz);
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
    for (Size r = 0; r < rows_; ++r) {
        for (Index c = outer_[r]; c < outer_[r + 1]; ++c) {
            ASSERT(inner_[c] >= 0);
            triplets.push_back(Triplet(Size(inner_[c]), r, data_[c]));
        }
    }

    std::sort(triplets.begin(), triplets.end()); // triplets must be sorted by row

    SparseMatrix tmp(cols_, rows_, triplets);

    swap(tmp);

    return *this;
}


SparseMatrix& SparseMatrix::prune(linalg::Scalar val) {

    std::vector<Scalar> v;
    std::vector<Index> inner;

    Size nnz = 0;
    for(Size r = 0; r < rows_; ++r) {
        const Index start = outer_[r];
        outer_[r] = Index(nnz);
        for(Index c = start; c < outer_[r + 1]; ++c)
            if(data_[c] != val) {
                v.push_back(data_[c]);
                inner.push_back(inner_[c]);
                ++nnz;
            }
    }
    outer_[rows_] = Index(nnz);

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

    Size rows;
    Size cols;
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


Stream& operator<<(Stream& s, const SparseMatrix& v) {
    v.encode(s);
    return s;
}


SparseMatrix::const_iterator SparseMatrix::const_iterator::operator++(int) {
    const_iterator it = *this;
    ++(*this);
    return it;
}


SparseMatrix::const_iterator& SparseMatrix::const_iterator::operator=(const SparseMatrix::const_iterator& other) {
    matrix_ = other.matrix_;
    index_  = other.index_;
    row_    = other.row_;
    return *this;
}

bool SparseMatrix::const_iterator::operator==(const SparseMatrix::const_iterator& other) const {
    ASSERT(other.matrix_ == matrix_);
    return other.index_ == index_;
}


SparseMatrix::const_iterator::const_iterator(const SparseMatrix& matrix) :
    matrix_(const_cast<SparseMatrix*>(&matrix)),
    index_(0),
    row_(0)
{
    const Index* outer = matrix_->outer();
    while(outer[row_+1] == 0) {
        ++row_;
    }
}

SparseMatrix::const_iterator::const_iterator(const SparseMatrix& matrix, Size row) :
    matrix_(const_cast<SparseMatrix*>(&matrix)),
    row_(row) {
    const Size rows = matrix_->rows_;
    if(row_ > rows) { row_ = rows; }
    index_ = Size(matrix_->outer_[row_]);
}

Size SparseMatrix::const_iterator::col() const {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return Size(matrix_->inner_[index_]);
}

Size SparseMatrix::const_iterator::row() const {
    return row_;
}


SparseMatrix::const_iterator& SparseMatrix::const_iterator::operator++() {
    if(lastOfRow()) {
        row_++;
    }
    index_++;
    return *this;
}


const Scalar& SparseMatrix::const_iterator::operator*() const {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return matrix_->data_[index_];
}

void eckit::linalg::SparseMatrix::const_iterator::print(std::ostream& os) const {
    os << "SparseMatrix::iterator(row=" << row_ << ", index=" << index_ << ")" << std::endl;
}


Scalar& SparseMatrix::iterator::operator*() {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return matrix_->data_[index_];
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace linalg
} // namespace eckit
