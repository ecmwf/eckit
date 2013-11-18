/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/BufferedHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#if 0
ClassSpec BufferedHandle::classSpec_ = {&DataHandle::classSpec(),"BufferedHandle",};
Reanimator<BufferedHandle> BufferedHandle::reanimator_;
#endif


BufferedHandle::BufferedHandle(DataHandle* h, size_t size):
	handle_(h),
	buffer_(size),
	pos_(0),
    size_(size),
    used_(0),
    eof_(false),
    read_(false),
    position_(0),
    owned_(true)
{
}

BufferedHandle::BufferedHandle(DataHandle& h, size_t size):
	handle_(&h),
	buffer_(size),
	pos_(0),
    size_(size),
    used_(0),
    eof_(false),
    read_(false),
    position_(0),
    owned_(false)
{
}

BufferedHandle::~BufferedHandle() 
{
    if( owned_ )
        delete handle_;
}

Length BufferedHandle::openForRead()
{
    read_ = true;
	used_ = pos_ = 0;
    eof_ = false;
    position_ = 0;
	return handle_->openForRead();
}

void BufferedHandle::openForWrite(const Length& length)
{
    read_ = false;
	pos_ = 0;
    position_ = 0;
	handle_->openForWrite(length);
}

void BufferedHandle::openForAppend(const Length& )
{
	NOTIMP;
}

long BufferedHandle::read(void* buffer,long length)
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
            used_   = handle_->read(buffer_,size_);
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

long BufferedHandle::write(const void* buffer,long length)
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
	return handle_->write(buffer,length);
}

void BufferedHandle::close()
{
	if(!read_) 
        bufferFlush();
    handle_->close();
}

void BufferedHandle::flush()
{
    bufferFlush();
    handle_->flush();
}

void BufferedHandle::rewind()
{
	used_ = pos_ = 0;
    eof_  = false;
	handle_->rewind();
}

void BufferedHandle::print(ostream& s) const
{
	s << "BufferedHandle[";
	handle_->print(s);
	s << ']';
}

Length BufferedHandle::estimate()
{
	return handle_->estimate();
}

Offset BufferedHandle::position()
{
    ASSERT(read_);
    return position_;
}

void BufferedHandle::bufferFlush()
{
	if(pos_)
	{
		long len = handle_->write(buffer_,pos_);
		ASSERT( (size_t) len == pos_ );
		pos_ = 0;
	}
}

string BufferedHandle::title() const {
    return string("{") + handle_->title() + "}";
}

//-----------------------------------------------------------------------------

} // namespace eckit

