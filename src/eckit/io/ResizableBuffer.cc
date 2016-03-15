/*
 * (C) Copyright 1996-2016 ECMWF.
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


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ResizableBuffer::ResizableBuffer(size_t size):
    buffer_(0),
    size_(size),
    fd_(-1)
{
        create();
}

ResizableBuffer::ResizableBuffer(const char* p,size_t size):
    buffer_(0),
    size_(size)
{
        create();
        ::memcpy(buffer_,p,size);
}

ResizableBuffer::ResizableBuffer(const std::string& s):
    buffer_(0),
    size_(s.length()+1)
{
        create();
        ::strcpy((char*)buffer_,s.c_str());
}

ResizableBuffer::~ResizableBuffer()
{
        destroy();
}

void ResizableBuffer::create()
{
//	Log::info() << "ResizableBuffer::create " << size_ << std::endl;
        if(size_)
        {
                fd_ = -1;
                buffer_ = ::mmap(0,size_,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,fd_,0);
                if(buffer_ == MAP_FAILED)
                    throw FailedSystemCall("mmap",Here());
        }
}

void ResizableBuffer::destroy()
{
//	Log::info() << "ResizableBuffer::destroy " << size_ << std::endl;
        if(size_)
        {
                munmap(buffer_,size_);
                if(fd_ >= 0) close(fd_);
                fd_ = -1;
        }
        buffer_ = 0;
}

void ResizableBuffer::resize(size_t size)
{
        destroy();
        size_ = size;
        create();
}

//-----------------------------------------------------------------------------

} // namespace eckit

