/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/MultiPartHandle.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

MultiPartHandle::MultiPartHandle(DataHandle* handle, const Length& size, MultiPartHandle* prev):
    handle_(handle),
    size_(size),
    prev_(prev),
    next_(0),
    position_(0),
    start_(0)
{
    if(prev_) {
        ASSERT(prev_->next_ == 0);
        prev_->next_ = this;
        start_ = prev_->start_ + prev_->size_;
    }
}

MultiPartHandle::~MultiPartHandle()
{
    if(next_ == 0) {
        delete handle_;
    }
}

Length MultiPartHandle::openForRead()
{
    if(prev_ == 0) {
        handle_->openForRead();
    }
    rewind();
    return estimate();
}

void MultiPartHandle::openForWrite(const Length& length)
{
    NOTIMP;
}

void MultiPartHandle::openForAppend(const Length& )
{
    NOTIMP;
}


long MultiPartHandle::read(void* buffer, long length)
{
    size_t left = size_ - position_;
    size_t size = std::min(left, size_t(length));

    size = handle_->read(buffer, size);
    position_ += size;

    return size;
}

long MultiPartHandle::write(const void* buffer, long length)
{
    NOTIMP;
}

void MultiPartHandle::close()
{
    if(next_ == 0) {
        handle_->close();
    }
}

void MultiPartHandle::flush()
{
    NOTIMP;
}

void MultiPartHandle::rewind()
{
    handle_->seek(start_);
    position_ = 0;
}

void MultiPartHandle::print(std::ostream& s) const
{

    if (format(s) == Log::compactFormat)
        s << "MultiPartHandle";
    else
    {
        s << "MultiPartHandle[";
        // for (size_t i = 0; i < datahandles_.size(); i++)
        // {
        //     if (i != 0)
        //         s << ",(";
            handle_->print(s);
        //     s << ")";
        // }
        s << ']';
    }
}

bool MultiPartHandle::merge(DataHandle* other)
{
    return false;
}

Length MultiPartHandle::estimate()
{
   return size_;
}

void MultiPartHandle::restartReadFrom(const Offset& from)
{
    NOTIMP;
}

void MultiPartHandle::toRemote(Stream &s) const
{
    NOTIMP;
}

void MultiPartHandle::toLocal(Stream &s) const
{
    NOTIMP;
}

DataHandle* MultiPartHandle::toLocal()
{
    NOTIMP;
}

void MultiPartHandle::cost(std::map<std::string, Length>& c, bool read) const
{
    NOTIMP;
}

bool MultiPartHandle::moveable() const
{
    return false;
}

std::string MultiPartHandle::title() const
{
    std::ostringstream os;
    os << "[";
    os << handle_->title();
    os << '|';
    os << start_;
    os << ':';
    os << size_;
    // if (datahandles_.size() > 0) os << datahandles_[0]->title();
    // if (datahandles_.size() > 1) os << ",...{" << datahandles_.size() << "}";
    os << "]";
    return os.str();
}

//-----------------------------------------------------------------------------

bool MultiPartHandle::compress(bool sorted) {
    return false;
}

//-----------------------------------------------------------------------------

} // namespace eckit
