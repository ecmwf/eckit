// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/linalg/allocator/StandardAllocator.h"

#include <ostream>

#include "eckit/log/Bytes.h"


namespace eckit::linalg::allocator {


StandardAllocator::StandardAllocator() : buffer_(0) {}


SparseMatrix::Layout StandardAllocator::allocate(SparseMatrix::Shape& shape) {
    if (shape.allocSize() > buffer_.size()) {
        buffer_.resize(shape.allocSize());
    }

    SparseMatrix::Layout layout;

    char* addr = buffer_;

    layout.data_  = reinterpret_cast<Scalar*>(addr);
    layout.outer_ = reinterpret_cast<SparseMatrix::UIndex*>(addr + shape.sizeofData());
    layout.inner_ = reinterpret_cast<Index*>(addr + shape.sizeofData() + shape.sizeofOuter());

    return layout;
}


void StandardAllocator::deallocate(SparseMatrix::Layout p, SparseMatrix::Shape) {}


bool StandardAllocator::inSharedMemory() const {
    return false;
}


void StandardAllocator::print(std::ostream& out) const {
    out << "StandardAllocator[" << Bytes{static_cast<double>(buffer_.size())} << "]";
}


}  // namespace eckit::linalg::allocator
