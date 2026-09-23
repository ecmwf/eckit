// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/linalg/SparseMatrix.h"


namespace eckit::linalg::allocator {


/**
 * @brief In-place allocator for sparse matrices, directly mapping supporting arrays including from another
 * SparseMatrix. It is able to provide a "view".
 */
class NonOwningAllocator : public SparseMatrix::Allocator {
public:

    NonOwningAllocator(Size Nr, Size Nc, Size nnz, Index* ia, Index* ja, Scalar* a);

    SparseMatrix::Layout allocate(SparseMatrix::Shape&) override;

    void deallocate(SparseMatrix::Layout, SparseMatrix::Shape) override;
    bool inSharedMemory() const override;
    void print(std::ostream&) const override;

private:

    const Size Nr_;
    const Size Nc_;
    const Size nnz_;
    Index* ia_;  // NOTE: not owned
    Index* ja_;  // NOTE: not owned
    Scalar* a_;  // NOTE: not owned
};


}  // namespace eckit::linalg::allocator
