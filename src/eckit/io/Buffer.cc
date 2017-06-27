/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/memory/MemoryPool.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(size_t size):
    buffer_(0),
    size_(size)
{
	create();
}

Buffer::Buffer(const char* p, size_t size):
    buffer_(0),
    size_(size)
{
    create();
    copy(p,size);
}

Buffer::Buffer(const std::string& s):
    buffer_(0),
    size_(s.length()+1)
{
    create();
    copy(s);
}


Buffer::~Buffer()
{
    destroy();
}

void Buffer::create()
{
    buffer_ = MemoryPool::largeAllocate(size_);
}

void Buffer::destroy()
{
    MemoryPool::largeDeallocate(buffer_);
}

void Buffer::copy(const std::string &s)
{
    ::strcpy(static_cast<char*>(buffer_), s.c_str());
}

void Buffer::copy(const char *p, size_t size)
{
    ::memcpy(buffer_,p,size);
}

void Buffer::swap(Buffer& rhs) {
    std::swap(buffer_, rhs.buffer_);
    std::swap(size_, rhs.size_);
}

void eckit::Buffer::resize(size_t size)
{
    destroy();
    size_ = size;
    create();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

