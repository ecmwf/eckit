/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/allocator/StandardContainerAllocator.h"

#include <numeric>
#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::linalg::allocator {


StandardContainerAllocator::StandardContainerAllocator(Size Nr, Size Nc, const container_type& cont) :
    Nr_(Nr), Nc_(Nc), nnz_([Nc](const auto& cont) {
        return std::accumulate(cont.begin(), cont.end(), 0, [Nc](const auto& acc, const auto& row) {
            ASSERT(row.empty() || row.rbegin()->first < Nc);
            return acc + row.size();
        });
    }(cont)) {
    ASSERT(Nr == cont.size());

    ia_.reserve(Nr_ + 1);
    ja_.reserve(nnz_);
    a_.reserve(nnz_);

    ia_.emplace_back(0);
    for (const auto& row : cont) {
        ia_.emplace_back(ia_.back() + row.size());
        for (const auto& [col, val] : row) {
            ja_.emplace_back(static_cast<Index>(col));
            a_.emplace_back(val);
        }
    }
}


SparseMatrix::Layout StandardContainerAllocator::StandardContainerAllocator::allocate(SparseMatrix::Shape& shape) {
    shape.size_ = nnz_;
    shape.rows_ = Nr_;
    shape.cols_ = Nc_;

    SparseMatrix::Layout layout;
    layout.outer_ = ia_.data();
    layout.inner_ = ja_.data();
    layout.data_  = a_.data();

    return layout;
}


void StandardContainerAllocator::deallocate(SparseMatrix::Layout, SparseMatrix::Shape) {
    ia_.clear();
    ja_.clear();
    a_.clear();
}


bool StandardContainerAllocator::inSharedMemory() const {
    return false;
}


void StandardContainerAllocator::print(std::ostream& out) const {
    out << "StandardContainerAllocator[Nr=" << Nr_ << ",Nc=" << Nc_ << ",nnz=" << nnz_ << "]";
}


}  // namespace eckit::linalg::allocator
