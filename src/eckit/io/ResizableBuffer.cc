/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <sys/mman.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/ResizableBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResizableBuffer::ResizableBuffer(size_t size):
    buffer_(allocate(size)),
    size_(size)
{
}

ResizableBuffer::ResizableBuffer(const char* p, size_t size):
    buffer_(allocate(size)),
    size_(size)
{
    ::memcpy(buffer_, p, size);
}

ResizableBuffer::~ResizableBuffer()
{
    deallocate(buffer_);
}

char* ResizableBuffer::allocate(size_t size)
{
    return new char[size];
}

void ResizableBuffer::deallocate(char* buffer)
{
    delete[] buffer;
}

void ResizableBuffer::resize(size_t size, bool preserveData)
{
    if (size != size_) {

        if (preserveData) {
            char *newbuffer = allocate(size);
            ::memcpy(newbuffer, buffer_, std::min(size_, size));
            deallocate(buffer_);
            size_ = size;
            buffer_ = newbuffer;
        }
        else {
            deallocate(buffer_);
            size_ = size;
            buffer_ = allocate(size);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

