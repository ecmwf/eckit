// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/linalg/SparseMatrix.h"
#include "eckit/memory/MemoryBuffer.h"


namespace eckit::linalg::allocator {


class BufferAllocator : public SparseMatrix::Allocator {
public:

    BufferAllocator(const MemoryBuffer&);

    SparseMatrix::Layout allocate(SparseMatrix::Shape&) override;

    void deallocate(SparseMatrix::Layout, SparseMatrix::Shape) override;
    bool inSharedMemory() const override;
    void print(std::ostream&) const override;

    MemoryBuffer buffer_;
};


}  // namespace eckit::linalg::allocator
