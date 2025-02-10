/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/allocator/BufferAllocator.h"

#include <ostream>

#include "eckit/log/Bytes.h"


namespace eckit::linalg::allocator {


BufferAllocator::BufferAllocator(const MemoryBuffer& buffer) : buffer_(buffer, buffer.size()) {}


SparseMatrix::Layout BufferAllocator::allocate(SparseMatrix::Shape& shape) {
    SparseMatrix::Layout layout;
    SparseMatrix::load(buffer_.data(), buffer_.size(), layout, shape);
    return layout;
}


void BufferAllocator::deallocate(SparseMatrix::Layout, SparseMatrix::Shape) {}


bool BufferAllocator::inSharedMemory() const {
    return false;
}


void BufferAllocator::print(std::ostream& out) const {
    out << "BufferAllocator[" << Bytes{static_cast<double>(buffer_.size())} << "]";
}


}  // namespace eckit::linalg::allocator
