/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/MemoryHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#if 0
ClassSpec MemoryHandle::classSpec_ = {&DataHandle::classSpec(),"MemoryHandle",};
Reanimator<MemoryHandle> MemoryHandle::reanimator_;
#endif


MemoryHandle::MemoryHandle(DataHandle* h, size_t size):
    HandleHolder(h),
	buffer_(size),
	pos_(0),
    size_(size),
    used_(0),
    eof_(false),
    read_(false),
    position_(0)
{
}

MemoryHandle::MemoryHandle(DataHandle& h, size_t size):
    HandleHolder(h),
	buffer_(size),
	pos_(0),
    size_(size),
    used_(0),
    eof_(false),
    read_(false),
    position_(0)
{
}

MemoryHandle::~MemoryHandle() 
{
}

Length MemoryHandle::openForRead()
{
    read_ = true;
	used_ = pos_ = 0;
    eof_ = false;
    position_ = 0;
    return handle().openForRead();
}

void MemoryHandle::openForWrite(const Length& length)
{
    read_ = false;
	pos_ = 0;
    position_ = 0;
    handle().openForWrite(length);
}

void MemoryHandle::openForAppend(const Length& )
{
	NOTIMP;
}

void MemoryHandle::skip(const Length& len)
{
    ASSERT(read_);
    unsigned long long left = used_ - pos_;
    unsigned long long n    = len;

    if(n < left) {
        position_ += n;
        pos_ += n;
        return;
    }

    seek(position() + len);
}

long MemoryHandle::read(void* buffer,long length)
{
    long len  = 0;
    long size = length;
    char *buf = (char*)buffer;

    ASSERT(read_);
        
    if(eof_)
        return -1;

    while(len < length && !eof_) {
        long left = used_ - pos_;
        ASSERT(left>=0);

        if(left == 0 && !eof_ )
        {
            used_   = handle().read(buffer_,size_);
            pos_    = 0;
            if(used_ <= 0) 
            {
                eof_ = true;
                len = len ? len : used_;
                if(len > 0) position_ += len;
				if(len == 0) return -1;
                return len;
            }
            left = used_;
        }

		char* p = buffer_;
        long s = size < left ? size : left;
		::memcpy(buf + len, p + pos_, s);
        len  += s; ASSERT(len <= length);
        pos_ += s; ASSERT(pos_ <= used_);
        size -= s; ASSERT(size >= 0);
    }

    if(len > 0) position_ += len;
	return len;
}

long MemoryHandle::write(const void* buffer,long length)
{
	long left = size_ - pos_;
	ASSERT(left >= 0);

    ASSERT(!read_);

	if(length > left)
	{
		bufferFlush();
		left = size_;
	}

	if(length <= left)
	{
		char* p = buffer_;
		::memcpy(p + pos_, buffer,length);
		pos_ += length; ASSERT(pos_ <= size_);
		return length;
	}

    ASSERT(pos_ == 0);
    return handle().write(buffer,length);
}

void MemoryHandle::close()
{
	if(!read_) 
        bufferFlush();
    handle().close();
}

void MemoryHandle::flush()
{
    bufferFlush();
    handle().flush();
}

void MemoryHandle::rewind()
{
	used_ = pos_ = 0;
    eof_  = false;
    handle().rewind();
}

Offset MemoryHandle::seek(const Offset& off)
{
    used_ = pos_ = 0;
    eof_  = false;
    position_ = handle().seek(off);
    return position_;
}


void MemoryHandle::print(std::ostream& s) const
{
	s << "MemoryHandle[";
    handle().print(s);
	s << ']';
}

Length MemoryHandle::estimate()
{
    return handle().estimate();
}

Offset MemoryHandle::position()
{
    ASSERT(read_);
    return position_;
}

void MemoryHandle::bufferFlush()
{
	if(pos_)
	{
        long len = handle().write(buffer_,pos_);
		ASSERT( (size_t) len == pos_ );
		pos_ = 0;
	}
}

std::string MemoryHandle::title() const {
    return std::string("{") + handle().title() + "}";
}

DataHandle* MemoryHandle::clone() const
{
    return new MemoryHandle(handle().clone(), buffer_.size());
}
//-----------------------------------------------------------------------------

} // namespace eckit

