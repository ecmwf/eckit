// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/linalg/SparseMatrix.h"
#include "eckit/memory/MemoryBuffer.h"


namespace eckit::linalg::allocator {


class StandardAllocator : public SparseMatrix::Allocator {
public:

    StandardAllocator();

    SparseMatrix::Layout allocate(SparseMatrix::Shape&) override;

    void deallocate(SparseMatrix::Layout, SparseMatrix::Shape) override;
    bool inSharedMemory() const override;
    void print(std::ostream&) const override;

    MemoryBuffer buffer_;
};


}  // namespace eckit::linalg::allocator
