/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


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
