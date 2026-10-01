// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/linalg/allocator/NonOwningAllocator.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::linalg::allocator {


NonOwningAllocator::NonOwningAllocator(Size Nr, Size Nc, Size nnz, Index* ia, Index* ja, Scalar* a) :
    Nr_(Nr), Nc_(Nc), nnz_(nnz), ia_(ia), ja_(ja), a_(a) {
    ASSERT(ia_ != nullptr);
    ASSERT(ja_ != nullptr);
    ASSERT(a_ != nullptr);
}


SparseMatrix::Layout NonOwningAllocator::NonOwningAllocator::allocate(SparseMatrix::Shape& shape) {
    shape.size_ = nnz_;
    shape.rows_ = Nr_;
    shape.cols_ = Nc_;

    SparseMatrix::Layout layout;
    layout.outer_ = reinterpret_cast<decltype(SparseMatrix::Layout::outer_)>(ia_);
    layout.inner_ = ja_;
    layout.data_  = a_;

    return layout;
}


void NonOwningAllocator::deallocate(SparseMatrix::Layout, SparseMatrix::Shape) {}


bool NonOwningAllocator::inSharedMemory() const {
    return false;
}


void NonOwningAllocator::print(std::ostream& out) const {
    out << "NonOwningAllocator[Nr=" << Nr_ << ",Nc=" << Nc_ << ",nnz=" << nnz_ << "]";
}


}  // namespace eckit::linalg::allocator
