/*
 * (C) Copyright 2024- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#pragma once

#include <map>
#include <vector>

#include "eckit/linalg/SparseMatrix.h"


namespace eckit::linalg::allocator {


/**
 * @brief In-place allocator for sparse matrices, directly mapping supporting arrays including from another
 * SparseMatrix. It is able to provide a "view".
 */
class StandardContainerAllocator : public SparseMatrix::Allocator {
public:

    using container_type = std::vector<std::map<Size, Scalar>>;

    StandardContainerAllocator(Size Nr, Size Nc, const container_type&);

    SparseMatrix::Layout allocate(SparseMatrix::Shape&) override;

    void deallocate(SparseMatrix::Layout, SparseMatrix::Shape) override;
    bool inSharedMemory() const override;
    void print(std::ostream&) const override;

private:

    const Size Nr_;
    const Size Nc_;
    const Size nnz_;

    std::vector<SparseMatrix::UIndex> ia_;
    std::vector<Index> ja_;
    std::vector<Scalar> a_;
};


}  // namespace eckit::linalg::allocator
