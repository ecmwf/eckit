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
#include <cassert>
#include <cstring>
#include <limits>
#include <ostream>

#include "eckit/eckit.h"  // for endianness

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/memory/MemoryBuffer.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/Stream.h"

namespace eckit::linalg {

// Extend range of Index for unsigned compression indices (should only be positive), preserving binary compatibility
static_assert(sizeof(Index) == sizeof(SparseMatrix::UIndex), "sizeof(sizeof(Index) == SparseMatrix::UIndex)");

static constexpr bool littleEndian = eckit_LITTLE_ENDIAN != 0;


//----------------------------------------------------------------------------------------------------------------------

namespace detail {

class StandardAllocator : public SparseMatrix::Allocator {
public:
    StandardAllocator() : membuff_(0) {}

    SparseMatrix::Layout allocate(SparseMatrix::Shape& shape) override {
        if (shape.allocSize() > membuff_.size()) {
            membuff_.resize(shape.allocSize());
        }

        SparseMatrix::Layout p;

        char* addr = membuff_;

        p.data_  = reinterpret_cast<Scalar*>(addr);
        p.outer_ = reinterpret_cast<SparseMatrix::UIndex*>(addr + shape.sizeofData());
        p.inner_ = reinterpret_cast<Index*>(addr + shape.sizeofData() + shape.sizeofOuter());

        return p;
    }

    void deallocate(SparseMatrix::Layout p, SparseMatrix::Shape) override {}
    bool inSharedMemory() const override { return false; }
    void print(std::ostream& out) const override {
        out << "StandardAllocator[" << Bytes{static_cast<double>(membuff_.size())} << "]";
    }

    MemoryBuffer membuff_;
};


class BufferAllocator : public SparseMatrix::Allocator {
public:
    BufferAllocator(const MemoryBuffer& buffer) : buffer_(buffer, buffer.size()) {}

    SparseMatrix::Layout allocate(SparseMatrix::Shape& shape) override {
        SparseMatrix::Layout layout;

        SparseMatrix::load(buffer_.data(), buffer_.size(), layout, shape);

        return layout;
    }

    void deallocate(SparseMatrix::Layout, SparseMatrix::Shape) override {}
    bool inSharedMemory() const override { return false; }
    void print(std::ostream& out) const override {
        out << "BufferAllocator[" << Bytes{static_cast<double>(buffer_.size())} << "]";
    }

    MemoryBuffer buffer_;
};


}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

void SparseMatrix::Shape::print(std::ostream& os) const {
    os << "Shape["
       << "nnz=" << size_ << ","
       << "rows=" << rows_ << ","
       << "cols=" << cols_ << "]";
}


SparseMatrix::SparseMatrix(Allocator* alloc) : owner_(alloc != nullptr ? alloc : new detail::StandardAllocator) {
    spm_ = owner_->allocate(shape_);
}


SparseMatrix::SparseMatrix(Size rows, Size cols, Allocator* alloc) :
    owner_(alloc != nullptr ? alloc : new detail::StandardAllocator) {
    reserve(rows, cols, 1);
}


SparseMatrix::SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets) :
    owner_(new detail::StandardAllocator) {

    // Count number of non-zeros, allocate memory 1 triplet per non-zero
    Size nnz = std::count_if(triplets.begin(), triplets.end(), [](const auto& tri) { return tri.nonZero(); });

    if (auto max = static_cast<Size>(std::numeric_limits<UIndex>::max()); max < nnz) {
        throw OutOfRange("SparseMatrix::SparseMatrix: too many non-zero entries, nnz=" + std::to_string(nnz)
                             + ", max=" + std::to_string(max),
                         Here());
    }

    reserve(rows, cols, nnz);

    Size pos = 0;
    Size row = 0;

    spm_.outer_[0] = 0; /* first entry (base) is always zero */

    // Build vectors of inner indices and values, update outer index per row
    for (const auto& tri : triplets) {
        if (tri.nonZero()) {

            // triplets are ordered by rows
            ASSERT(tri.row() >= row);
            ASSERT(tri.row() < shape_.rows_);
            ASSERT(tri.col() < shape_.cols_);

            // start a new row
            while (tri.row() > row) {
                spm_.outer_[++row] = static_cast<UIndex>(pos);
            }

            spm_.inner_[pos] = static_cast<Index>(tri.col());
            spm_.data_[pos]  = tri.value();
            ++pos;
        }
    }

    while (row < shape_.rows_) {
        spm_.outer_[++row] = static_cast<UIndex>(pos);
    }

    ASSERT(static_cast<Size>(spm_.outer_[shape_.outerSize() - 1]) == nonZeros());
}


SparseMatrix::SparseMatrix(Stream& s) : owner_(new detail::StandardAllocator) {
    decode(s);
}


SparseMatrix::SparseMatrix(const MemoryBuffer& buffer) : owner_(new detail::BufferAllocator(buffer)) {
    spm_ = owner_->allocate(shape_);
}


SparseMatrix::SparseMatrix(const SparseMatrix& other) : owner_(new detail::StandardAllocator) {
    if (!other.empty()) {  // in case we copy an other that was constructed empty

        reserve(other.rows(), other.cols(), other.nonZeros());

        std::memcpy(spm_.data_, other.spm_.data_, shape_.sizeofData());
        std::memcpy(spm_.outer_, other.spm_.outer_, shape_.sizeofOuter());
        std::memcpy(spm_.inner_, other.spm_.inner_, shape_.sizeofInner());
    }
}


SparseMatrix::SparseMatrix(SparseMatrix&& other) {
    swap(other);
}


SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other) {
    SparseMatrix copy(other);
    swap(copy);
    return *this;
}


SparseMatrix& SparseMatrix::operator=(SparseMatrix&& other) {
    swap(other);
    return *this;
}


SparseMatrix::~SparseMatrix() {
    reset();
}


void SparseMatrix::reset() {
    if (owner_) {
        owner_->deallocate(spm_, shape_);
    }

    spm_.reset();
    shape_.reset();
}


void SparseMatrix::reserve(Size rows, Size cols, Size nnz) {
    ASSERT(nnz > 0);
    ASSERT(nnz <= rows * cols);
    ASSERT(rows > 0 && cols > 0);

    reset();

    shape_.rows_ = rows;
    shape_.cols_ = cols;
    shape_.size_ = nnz;

    spm_ = owner_->allocate(shape_);
}


void SparseMatrix::save(const PathName& path) const {
    FileStream s(path, "w");
    auto c = closer(s);
    encode(s);
}


void SparseMatrix::load(const PathName& path) {
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
    const auto* b = static_cast<const char*>(buffer);

    MemoryHandle mh(buffer, bufferSize);
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

    auto* addr = const_cast<char*>(b);

    layout.data_  = reinterpret_cast<Scalar*>(addr + info.data_);
    layout.outer_ = reinterpret_cast<UIndex*>(addr + info.outer_);
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

    ASSERT(mh.write(spm_.data_, shape_.sizeofData()) == static_cast<long>(shape_.sizeofData()));
    ASSERT(mh.write(spm_.outer_, shape_.sizeofOuter()) == static_cast<long>(shape_.sizeofOuter()));
    ASSERT(mh.write(spm_.inner_, shape_.sizeofInner()) == static_cast<long>(shape_.sizeofInner()));
}


void SparseMatrix::swap(SparseMatrix& other) {
    std::swap(spm_, other.spm_);
    std::swap(shape_, other.shape_);

    owner_.swap(other.owner_);
}


const Index* SparseMatrix::outer() const {
    if (auto max = static_cast<Size>(std::numeric_limits<Index>::max()); max < nonZeros()) {
        throw OutOfRange("SparseMatrix::outer: too many non-zero entries, nnz=" + std::to_string(nonZeros())
                             + ", max=" + std::to_string(max) + " (for Index-typed compatibility)",
                         Here());
    }

    return reinterpret_cast<Index*>(spm_.outer_);
}


void SparseMatrix::cols(Size cols) {
    ASSERT(cols > 0);
    shape_.cols_ = cols;
}


size_t SparseMatrix::footprint() const {
    return sizeof(*this) + shape_.allocSize();
}


bool SparseMatrix::inSharedMemory() const {
    ASSERT(owner_);
    return owner_->inSharedMemory();
}


void SparseMatrix::dump(std::ostream& os) const {
    for (Size i = 0; i < rows(); ++i) {
        const_iterator itr  = begin(i);
        const_iterator iend = end(i);

        if (itr == iend) {
            continue;
        }
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

    auto nnz = std::min(rows, cols);

    reserve(rows, cols, nnz);

    for (Size i = 0; i < nnz; ++i) {
        spm_.outer_[i] = static_cast<UIndex>(i);
        spm_.inner_[i] = static_cast<Index>(i);
    }

    for (Size i = nnz; i <= shape_.rows_; ++i) {
        spm_.outer_[i] = static_cast<UIndex>(nnz);
    }

    for (Size i = 0; i < shape_.size_; ++i) {
        spm_.data_[i] = static_cast<Scalar>(1);
    }

    return *this;
}


SparseMatrix& SparseMatrix::transpose() {
    /// @note Can SparseMatrix::transpose() be done more efficiently?
    ///       We are building another matrix and then swapping

    std::vector<Triplet> triplets;
    triplets.reserve(nonZeros());

    for (Size r = 0; r < shape_.rows_; ++r) {
        for (auto c = spm_.outer_[r]; c < spm_.outer_[r + 1]; ++c) {
            ASSERT(spm_.inner_[c] >= 0);
            triplets.emplace_back(static_cast<Size>(spm_.inner_[c]), r, spm_.data_[c]);
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

        for (auto itr = begin(row), iend = end(row); itr != iend; ++itr) {
            triplets.emplace_back(newrow, itr.col(), *itr);
        }
    }

    return {p.size(), cols(), triplets};
}


SparseMatrix& SparseMatrix::prune(Scalar val) {
    std::vector<Scalar> v;
    std::vector<Index> inner;

    Size nnz = 0;
    for (Size r = 0; r < shape_.rows_; ++r) {
        const auto start = spm_.outer_[r];
        spm_.outer_[r]   = static_cast<UIndex>(nnz);
        for (auto c = start; c < spm_.outer_[r + 1]; ++c) {
            if (spm_.data_[c] != val) {
                v.push_back(spm_.data_[c]);
                inner.push_back(spm_.inner_[c]);
                ++nnz;
            }
        }
    }
    spm_.outer_[shape_.rows_] = static_cast<UIndex>(nnz);

    SparseMatrix tmp;
    tmp.reserve(shape_.rows_, shape_.cols_, nnz);

    std::memcpy(tmp.spm_.data_, v.data(), nnz * sizeof(Scalar));
    std::memcpy(tmp.spm_.outer_, spm_.outer_, shape_.outerSize() * sizeof(UIndex));
    std::memcpy(tmp.spm_.inner_, inner.data(), nnz * sizeof(Index));

    swap(tmp);

    return *this;
}


const SparseMatrix::Allocator& SparseMatrix::owner() const {
    ASSERT(owner_);
    return *owner_;
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

    s.writeLargeBlob(spm_.outer_, shape_.outerSize() * sizeof(UIndex));
    s.writeLargeBlob(spm_.inner_, shape_.innerSize() * sizeof(Index));
    s.writeLargeBlob(spm_.data_, shape_.dataSize() * sizeof(Scalar));
}


void SparseMatrix::decode(Stream& s) {
    Size rows = 0;
    Size cols = 0;
    Size nnz  = 0;

    s >> rows;
    s >> cols;
    s >> nnz;

    bool little_endian = true;
    s >> little_endian;
    ASSERT(littleEndian == little_endian);

    size_t index_size = 0;
    s >> index_size;
    ASSERT(index_size == sizeof(Index));

    size_t scalar_size = 0;
    s >> scalar_size;
    ASSERT(scalar_size == sizeof(Scalar));

    size_t size_size = 0;
    s >> size_size;
    ASSERT(size_size == sizeof(Size));

    reset();

    owner_ = std::make_unique<detail::StandardAllocator>();

    reserve(rows, cols, nnz);

    Log::debug<LibEcKit>() << "Decoding matrix : "
                           << " rows " << rows << " cols " << cols << " nnz " << nnz << " footprint " << footprint()
                           << std::endl;

    s.readLargeBlob(spm_.outer_, shape_.outerSize() * sizeof(UIndex));
    s.readLargeBlob(spm_.inner_, shape_.innerSize() * sizeof(Index));
    s.readLargeBlob(spm_.data_, shape_.dataSize() * sizeof(Scalar));
}


Stream& operator<<(Stream& s, const SparseMatrix& v) {
    v.encode(s);
    return s;
}


SparseMatrix::const_iterator SparseMatrix::const_iterator::operator++(int) {
    auto it = *this;
    ++(*this);
    return it;
}


bool SparseMatrix::const_iterator::operator==(const SparseMatrix::const_iterator& other) const {
    ASSERT(other.matrix_ == matrix_);
    return other.index_ == index_;
}


SparseMatrix::const_iterator::const_iterator(const SparseMatrix& matrix) :
    matrix_(const_cast<SparseMatrix*>(&matrix)), index_(0) {
    for (row_ = 0; matrix_->spm_.outer_[row_ + 1] == 0;) {
        ++row_;
    }
}


SparseMatrix::const_iterator::const_iterator(const SparseMatrix& matrix, Size row) :
    matrix_(const_cast<SparseMatrix*>(&matrix)), row_(row) {
    if (const Size rows = matrix_->rows(); row_ > rows) {
        row_ = rows;
    }
    index_ = static_cast<Size>(matrix_->spm_.outer_[row_]);
}


Size SparseMatrix::const_iterator::col() const {
    ASSERT(matrix_ && index_ < matrix_->nonZeros());
    return static_cast<Size>(matrix_->inner()[index_]);
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
    return matrix_->data()[index_];
}


void SparseMatrix::const_iterator::print(std::ostream& os) const {
    os << "SparseMatrix::iterator(row=" << row_ << ", col=" << col() << ", index=" << index_
       << ", value=" << operator*() << ")" << std::endl;
}


Scalar& SparseMatrix::iterator::operator*() {
    assert(matrix_ && index_ < matrix_->nonZeros());
    return matrix_->spm_.data_[index_];
}

//----------------------------------------------------------------------------------------------------------------------

SparseMatrix::Allocator::~Allocator() = default;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
