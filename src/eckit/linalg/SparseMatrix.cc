/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/SparseMatrix.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <numeric>

#include "eckit/eckit.h"  // for endianness

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/memory/MemoryBuffer.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/Stream.h"


namespace eckit {
namespace linalg {

#ifdef ECKIT_LITTLE_ENDIAN
static const bool littleEndian = true;
#else
static const bool littleEndian = false;
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace detail {

class StandardAllocator : public SparseMatrix::Allocator {
public:
    StandardAllocator() :
        membuff_(0) {}

    virtual SparseMatrix::Layout allocate(SparseMatrix::Shape& shape) {

        if (shape.allocSize() > membuff_.size()) {
            membuff_.resize(shape.allocSize());
        }

        SparseMatrix::Layout p;

        char* addr = membuff_;

        p.data_  = reinterpret_cast<Scalar*>(addr);
        p.outer_ = reinterpret_cast<Index*>(addr + shape.sizeofData());
        p.inner_ = reinterpret_cast<Index*>(addr + shape.sizeofData() + shape.sizeofOuter());

        return p;
    }

    virtual void deallocate(SparseMatrix::Layout p, SparseMatrix::Shape) {}

    virtual bool inSharedMemory() const { return false; }

    virtual void print(std::ostream& out) const { out << "StandardAllocator[" << Bytes(membuff_.size()) << "]"; }

    eckit::MemoryBuffer membuff_;
};

class BufferAllocator : public SparseMatrix::Allocator {
public:
    BufferAllocator(const MemoryBuffer& buffer) :
        buffer_(buffer, buffer.size()) {}

    virtual eckit::linalg::SparseMatrix::Layout allocate(eckit::linalg::SparseMatrix::Shape& shape) {

        using namespace eckit::linalg;

        SparseMatrix::Layout layout;

        eckit::linalg::SparseMatrix::load(buffer_.data(), buffer_.size(), layout, shape);

        return layout;
    }

    virtual void deallocate(eckit::linalg::SparseMatrix::Layout, eckit::linalg::SparseMatrix::Shape) {}

    virtual bool inSharedMemory() const { return false; }

    virtual void print(std::ostream& out) const { out << "BufferAllocator[" << Bytes(buffer_.size()) << "]"; }

    MemoryBuffer buffer_;
};

}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

SparseMatrix::SparseMatrix(Allocator* alloc) {
    owner_.reset(alloc ? alloc : new detail::StandardAllocator());
    spm_ = owner_->allocate(shape_);
}

SparseMatrix::SparseMatrix(Size rows, Size cols, Allocator* alloc) {
    owner_.reset(alloc ? alloc : new detail::StandardAllocator());
    reserve(rows, cols, 1);
}

SparseMatrix::SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets) :
    owner_(new detail::StandardAllocator()) {

    // Count number of non-zeros
    Size nnz{0};
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it) {
        if (it->nonZero()) {
            ++nnz;
        }
    }

    reserve(rows, cols, nnz);  // allocate memory 1 triplet per non-zero

    Size pos = 0;
    Size row = 0;

    spm_.outer_[0] = 0; /* first entry is always zero */

    // Build vectors of inner indices and values, update outer index per row
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it) {

        if (it->nonZero()) {

            // triplets are ordered by rows
            ASSERT(it->row() >= row);
            ASSERT(it->row() < shape_.rows_);
            // ASSERT( it->col() >= 0 ); // useless comparison with unsigned int
            ASSERT(it->col() < shape_.cols_);

            // start a new row
            while (it->row() > row) {
                spm_.outer_[++row] = Index(pos);
            }

            spm_.inner_[pos] = Index(it->col());
            spm_.data_[pos]  = it->value();
            ++pos;
        }
    }

    while (row < shape_.rows_) {
        spm_.outer_[++row] = Index(pos);
    }

    ASSERT(Size(spm_.outer_[shape_.outerSize() - 1]) == nonZeros()); /* last entry is always the nnz */
}


SparseMatrix::SparseMatrix(Stream& s) {
    owner_.reset(new detail::StandardAllocator());
    decode(s);
}

SparseMatrix::SparseMatrix(const MemoryBuffer& buffer) {
    owner_.reset(new detail::BufferAllocator(buffer));
    spm_ = owner_->allocate(shape_);
}

SparseMatrix::SparseMatrix(const SparseMatrix& other) {

    owner_.reset(new detail::StandardAllocator());

    if (!other.empty()) {  // in case we copy an other that was constructed empty

        reserve(other.rows(), other.cols(), other.nonZeros());

        ::memcpy(spm_.data_, other.spm_.data_, shape_.sizeofData());
        ::memcpy(spm_.outer_, other.spm_.outer_, shape_.sizeofOuter());
        ::memcpy(spm_.inner_, other.spm_.inner_, shape_.sizeofInner());
    }
}

SparseMatrix::SparseMatrix(SparseMatrix&& other) :
    SparseMatrix() {
    swap(other);
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other) {
    SparseMatrix copy(other);
    swap(copy);
    return *this;
}

SparseMatrix::~SparseMatrix() {
    reset();
}

void SparseMatrix::reset() {

    owner_->deallocate(spm_, shape_);

    spm_.reset();
    shape_.reset();
}


// variables into this method must be by value
void SparseMatrix::reserve(Size rows, Size cols, Size nnz) {

    ASSERT(nnz > 0);
    ASSERT(nnz <= rows * cols);
    ASSERT(rows > 0 && cols > 0); /* rows and columns must have some size */

    reset();

    shape_.rows_ = rows;
    shape_.cols_ = cols;
    shape_.size_ = nnz;

    spm_ = owner_->allocate(shape_);
}


void SparseMatrix::save(const eckit::PathName& path) const {
    FileStream s(path, "w");
    auto c = closer(s);
    encode(s);
}


void SparseMatrix::load(const eckit::PathName& path) {
    FileStream s(path, "r");
    auto c = closer(s);
    decode(s);
}

struct SPMInfo {
    size_t size_;  ///< non-zeros
    size_t rows_;  ///< rows
    size_t cols_;  ///< columns
    ptrdiff_t data_;
    ptrdiff_t outer_;
    ptrdiff_t inner_;
};

void SparseMatrix::load(const void* buffer, size_t bufferSize, Layout& layout, Shape& shape) {
    const char* b = static_cast<const char*>(buffer);

    eckit::MemoryHandle mh(buffer, bufferSize);
    mh.openForRead();

    struct SPMInfo info;
    mh.read(&info, sizeof(SPMInfo));

    ASSERT(info.size_ && info.rows_ && info.cols_);
    ASSERT(info.data_ > 0 && info.outer_ > 0 && info.inner_ > 0);

    // check that shape is matching what we are loading

    shape.size_ = info.size_;
    shape.rows_ = info.rows_;
    shape.cols_ = info.cols_;

    Log::debug<LibEcKit>() << "Loading matrix from buffer: "
                           << " rows " << shape.rows_ << " cols " << shape.cols_ << " nnzs " << shape.size_
                           << " allocSize " << shape.allocSize() << std::endl;

    ASSERT(bufferSize >= sizeof(SPMInfo) + shape.sizeofData() + shape.sizeofOuter() + shape.sizeofInner());

    char* addr = const_cast<char*>(b);

    layout.data_  = reinterpret_cast<Scalar*>(addr + info.data_);
    layout.outer_ = reinterpret_cast<Index*>(addr + info.outer_);
    layout.inner_ = reinterpret_cast<Index*>(addr + info.inner_);

    // check offsets don't segfault

    ASSERT(info.data_ + shape.sizeofData() <= bufferSize);
    ASSERT(info.outer_ + shape.sizeofOuter() <= bufferSize);
    ASSERT(info.inner_ + shape.sizeofInner() <= bufferSize);
}

void SparseMatrix::dump(MemoryBuffer& buffer) const {
    SparseMatrix::dump(buffer.data(), buffer.size());
}

void SparseMatrix::dump(void* buffer, size_t size) const {

    size_t minimum = sizeof(SPMInfo) + shape_.sizeofData() + shape_.sizeofOuter() + shape_.sizeofInner();
    ASSERT(size >= minimum);

    MemoryHandle mh(buffer, size);
    mh.openForWrite(size);

    SPMInfo info;

    info.size_ = nonZeros();
    info.rows_ = rows();
    info.cols_ = cols();

    info.data_  = sizeof(SPMInfo);
    info.outer_ = info.data_ + shape_.sizeofData();
    info.inner_ = info.outer_ + shape_.sizeofOuter();

    Log::debug<LibEcKit>() << "Dumping matrix : "
                           << " rows " << info.rows_ << " cols " << info.cols_ << " nnzs " << info.size_
                           << " allocSize " << shape_.allocSize() << std::endl;

    /// @todo we should try to get these memory aligned (to say 64 bytes)

    mh.write(&info, sizeof(SPMInfo));

    ASSERT(mh.write(spm_.data_, shape_.sizeofData()) == long(shape_.sizeofData()));
    ASSERT(mh.write(spm_.outer_, shape_.sizeofOuter()) == long(shape_.sizeofOuter()));
    ASSERT(mh.write(spm_.inner_, shape_.sizeofInner()) == long(shape_.sizeofInner()));
}

void SparseMatrix::swap(SparseMatrix& other) {

    std::swap(spm_, other.spm_);
    std::swap(shape_, other.shape_);

    owner_.swap(other.owner_);
}

void SparseMatrix::cols(Size cols) {
    ASSERT(cols > 0);
    shape_.cols_ = cols;
}

size_t SparseMatrix::footprint() const {
    return sizeof(*this) + shape_.allocSize();
}

bool SparseMatrix::inSharedMemory() const {
    ASSERT(owner_.get());
    return owner_->inSharedMemory();
}

void SparseMatrix::dump(std::ostream& os) const {
    for (Size i = 0; i < rows(); ++i) {

        const_iterator itr  = begin(i);
        const_iterator iend = end(i);

        if (itr == iend)
            continue;
        os << itr.row();

        for (; itr != iend; ++itr) {
            os << " " << itr.col() << " " << *itr;
        }
        os << std::endl;
    }
}

void SparseMatrix::print(std::ostream& os) const {
    os << "SparseMatrix[" << shape_ << "," << *owner_ << "]";
}

SparseMatrix& SparseMatrix::setIdentity(Size rows, Size cols) {

    ASSERT(rows > 0 && cols > 0);

    Size nnz = std::min(rows, cols);

    reserve(rows, cols, nnz);

    for (Size i = 0; i < nnz; ++i) {
        spm_.outer_[i] = Index(i);
        spm_.inner_[i] = Index(i);
    }

    for (Size i = nnz; i <= shape_.rows_; ++i) {
        spm_.outer_[i] = Index(nnz);
    }

    for (Size i = 0; i < shape_.size_; ++i) {
        spm_.data_[i] = Scalar(1);
    }

    return *this;
}


SparseMatrix& SparseMatrix::transpose() {

    /// @note Can SparseMatrix::transpose() be done more efficiently?
    ///       We are building another matrix and then swapping

    std::vector<Triplet> triplets;
    triplets.reserve(nonZeros());
    for (Size r = 0; r < shape_.rows_; ++r) {
        for (Index c = spm_.outer_[r]; c < spm_.outer_[r + 1]; ++c) {
            ASSERT(spm_.inner_[c] >= 0);
            triplets.push_back(Triplet(Size(spm_.inner_[c]), r, spm_.data_[c]));
        }
    }

    std::sort(triplets.begin(), triplets.end());  // triplets must be sorted by row

    SparseMatrix tmp(shape_.cols_, shape_.rows_, triplets);

    swap(tmp);

    return *this;
}

SparseMatrix SparseMatrix::rowReduction(const std::vector<size_t>& p) const {
    ASSERT(p.size() <= rows());

    std::vector<Triplet> triplets;

    for (size_t newrow = 0; newrow < p.size(); ++newrow) {

        size_t row = p[newrow];

        const_iterator itr  = begin(row);
        const_iterator iend = end(row);

        if (itr == iend)
            continue;

        for (; itr != iend; ++itr) {
            triplets.push_back(Triplet(newrow, itr.col(), *itr));
        }
    }

    return SparseMatrix(p.size(), cols(), triplets);
}


SparseMatrix& SparseMatrix::prune(linalg::Scalar val) {

    std::vector<Scalar> v;
    std::vector<Index> inner;

    Size nnz = 0;
    for (Size r = 0; r < shape_.rows_; ++r) {
        const Index start = spm_.outer_[r];
        spm_.outer_[r]    = Index(nnz);
        for (Index c = start; c < spm_.outer_[r + 1]; ++c)
            if (spm_.data_[c] != val) {
                v.push_back(spm_.data_[c]);
                inner.push_back(spm_.inner_[c]);
                ++nnz;
            }
    }
    spm_.outer_[shape_.rows_] = Index(nnz);

    SparseMatrix tmp;
    tmp.reserve(shape_.rows_, shape_.cols_, nnz);

    ::memcpy(tmp.spm_.data_, v.data(), nnz * sizeof(Scalar));
    ::memcpy(tmp.spm_.outer_, spm_.outer_, shape_.outerSize() * sizeof(Index));
    ::memcpy(tmp.spm_.inner_, inner.data(), nnz * sizeof(Index));

    swap(tmp);

    return *this;
}

const SparseMatrix::Allocator& SparseMatrix::owner() const {
    ASSERT(owner_.get());
    return *(owner_.get());
}


void SparseMatrix::encode(Stream& s) const {

    s << shape_.rows_;
    s << shape_.cols_;
    s << shape_.size_;

    s << littleEndian;
    s << sizeof(Index);
    s << sizeof(Scalar);
    s << sizeof(Size);

    Log::debug<LibEcKit>() << "Encoding matrix : "
                           << " rows " << rows() << " cols " << cols() << " nnz " << nonZeros() << " footprint "
                           << footprint() << std::endl;

    s.writeLargeBlob(spm_.outer_, shape_.outerSize() * sizeof(Index));
    s.writeLargeBlob(spm_.inner_, shape_.innerSize() * sizeof(Index));
    s.writeLargeBlob(spm_.data_, shape_.dataSize() * sizeof(Scalar));
}


void SparseMatrix::decode(Stream& s) {

    Size rows;
    Size cols;
    Size nnz;

    s >> rows;
    s >> cols;
    s >> nnz;

    bool little_endian;
    s >> little_endian;
    ASSERT(littleEndian == little_endian);

    size_t index_size;
    s >> index_size;
    ASSERT(index_size == sizeof(Index));

    size_t scalar_size;
    s >> scalar_size;
    ASSERT(scalar_size == sizeof(Scalar));

    size_t size_size;
    s >> size_size;
    ASSERT(size_size == sizeof(Size));

    reset();

    owner_.reset(new detail::StandardAllocator());

    reserve(rows, cols, nnz);

    Log::debug<LibEcKit>() << "Decoding matrix : "
                           << " rows " << rows << " cols " << cols << " nnz " << nnz << " footprint " << footprint()
                           << std::endl;

    s.readLargeBlob(spm_.outer_, shape_.outerSize() * sizeof(Index));
    s.readLargeBlob(spm_.inner_, shape_.innerSize() * sizeof(Index));
    s.readLargeBlob(spm_.data_, shape_.dataSize() * sizeof(Scalar));
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
    matrix_(const_cast<SparseMatrix*>(&matrix)), index_(0), row_(0) {
    const Index* outer = matrix_->outer();
    while (outer[row_ + 1] == 0) {
        ++row_;
    }
}

SparseMatrix::const_iterator::const_iterator(const SparseMatrix& matrix, Size row) :
    matrix_(const_cast<SparseMatrix*>(&matrix)), row_(row) {
    const Size rows = matrix_->rows();
    if (row_ > rows) {
        row_ = rows;
    }
    index_ = Size(matrix_->outer()[row_]);
}

Size SparseMatrix::const_iterator::col() const {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return Size(matrix_->inner()[index_]);
}

Size SparseMatrix::const_iterator::row() const {
    return row_;
}


SparseMatrix::const_iterator& SparseMatrix::const_iterator::operator++() {
    if (lastOfRow()) {
        row_++;
    }
    index_++;
    return *this;
}


const Scalar& SparseMatrix::const_iterator::operator*() const {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return matrix_->spm_.data_[index_];
}

void eckit::linalg::SparseMatrix::const_iterator::print(std::ostream& os) const {
    os << "SparseMatrix::iterator(row=" << row_ << ", index=" << index_ << ")" << std::endl;
}


Scalar& SparseMatrix::iterator::operator*() {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return matrix_->spm_.data_[index_];
}

//----------------------------------------------------------------------------------------------------------------------

SparseMatrix::Allocator::~Allocator() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
