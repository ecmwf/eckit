/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/memory/MemoryPool.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Buffer::Buffer(size_t size):
    owned_(true),
	size_(size),
	buffer_(0)
{
	create();
}

Buffer::Buffer(const char* p, size_t size):
    owned_(true),
    size_(size),
    buffer_(0)
{
    create();
    copy(p,size);
}

Buffer::Buffer(void* p, size_t size, bool dummy):
    owned_(false),
    size_(size),
    buffer_(p)
{
}

Buffer::Buffer(const std::string& s):
    owned_(true),
    size_(s.length()+1),
	buffer_(0)
{
	create();
    copy(s);
}


Buffer::~Buffer()
{ 
    if(owned_)
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
    ::strcpy((char*)buffer_,s.c_str());
}

void Buffer::copy(const char *p, size_t size)
{
    ::memcpy(buffer_,p,size);
}

//-----------------------------------------------------------------------------

} // namespace eckit

