/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eclib/MemoryPool.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Buffer::Buffer(size_t size):
	size_(size),
	buffer_(0)
{
	create();
}

Buffer::Buffer(const char* p,size_t size):
	size_(size),
	buffer_(0)
{
	create();
	::memcpy(buffer_,p,size);
}

Buffer::Buffer(const string& s):
	size_(s.length()+1),
	buffer_(0)
{
	create();
	::strcpy((char*)buffer_,s.c_str());
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

//-----------------------------------------------------------------------------

} // namespace eckit

