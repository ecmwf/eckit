/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/config/Resource.h"
#include "eckit/compat/StrStream.h"

#include "eckit/io/MarsFSPartHandle.h"
#include "eckit/io/PartHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec PartHandle::classSpec_ = {&DataHandle::classSpec(),"PartHandle",};
Reanimator<PartHandle> PartHandle::reanimator_;

void PartHandle::print(std::ostream& s) const
{
    if(format(s) == Log::compactFormat)
        s << "PartHandle";
    else
        s << "PartHandle[handle=" << *handle_
           << ",offset=" << offset_
           << ",length=" << length_ << ']';
}

void PartHandle::encode(Stream& s) const
{
    DataHandle::encode(s);
    s << *handle_;
    s << offset_;
    s << length_;

}

PartHandle::PartHandle(Stream& s):
    DataHandle(s),
    ownHandle_(true)
{
    handle_ = Reanimator<DataHandle>::reanimate(s);
    s >> offset_;
    s >> length_;

    ASSERT(offset_.size() == length_.size());
}


PartHandle::PartHandle(DataHandle& handle,
    const OffsetList& offset,const LengthList& length):
    handle_(&handle),
    ownHandle_(false),
    offset_(offset),
    length_(length)
{
//    Log::info() << "PartHandle::PartHandle " << name << std::endl;
    ASSERT(offset_.size() == length_.size());
    compress(false);
}

PartHandle::PartHandle(DataHandle* handle,
    const OffsetList& offset,const LengthList& length):
    handle_(handle),
    ownHandle_(true),
    offset_(offset),
    length_(length)
{
//    Log::info() << "PartHandle::PartHandle " << name << std::endl;
    ASSERT(offset_.size() == length_.size());
    compress(false);
}

PartHandle::PartHandle(DataHandle& handle,
    const Offset& offset,const Length& length):
    handle_(&handle),
    ownHandle_(false),
    offset_(1,offset),
    length_(1,length)
{
}

PartHandle::PartHandle(DataHandle* handle,
    const Offset& offset,const Length& length):
    handle_(handle),
    ownHandle_(true),
    offset_(1,offset),
    length_(1,length)
{
}

bool PartHandle::compress(bool sorted)
{
    if(sorted)
        eckit::sort(offset_,length_);
    return eckit::compress(offset_,length_);
}

PartHandle::~PartHandle()
{
    if(ownHandle_)
    {
       delete handle_;
    }
}

Length PartHandle::openForRead()
{
    handle_->openForRead();
    return estimate();
}

void PartHandle::openForWrite(const Length&)
{
    NOTIMP;
}

void PartHandle::openForAppend(const Length&)
{
    NOTIMP;
}

long PartHandle::read1(char *buffer,long length)
{
    // skip empty entries if any
    while (index_ < offset_.size() && length_[index_] == Length(0))
        index_++;

    if(index_ == offset_.size()) return 0;


    Length ll  = (long long)offset_[index_] + Length(pos_);
    off_t pos = ll;


    ASSERT(handle_->seek(pos) == Offset(pos));


    ll        = length_[index_] - Length(pos_);
    Length lsize = std::min(Length(length),ll);
    long size  = lsize;

    ASSERT(Length(size) == lsize);

    long n = handle_->read(buffer,size);

    if(n != size)
    {
        StrStream s;
        s << handle_ << ": cannot read " << size << ", got only " << n << StrStream::ends;
        throw ReadError(std::string(s));
    }

    pos_ += n;
    if(pos_ >= length_[index_])
    {
        index_++;
        pos_ = 0;
    }

    return n;
}


long PartHandle::read(void* buffer,long length)
{
    char *p = (char*)buffer;

    long n = 0;
    long total = 0;

    while( length > 0 && (n = read1(p,length)) >0)
    {
        length -= n;
        total  += n;
        p      += n;
    }

    return total>0?total:n;

}

long PartHandle::write(const void* buffer,long length)
{
    return -1;
}

void PartHandle::close()
{
    handle_->close();
}

void PartHandle::rewind()
{
    pos_   = 0;
    index_ = 0;
}

void PartHandle::restartReadFrom(const Offset& from)
{
    Log::warning() << *this << " restart read from " << from << std::endl;
    rewind();
    long long len = from;
    long long pos = 0;

    for(index_ = 0; index_ < length_.size(); index_++)
    {
        long long e = length_[index_];
        if(len >= pos && len < pos + e)
        {
            Log::warning() << *this << " restart read from " << from << ", index=" << index_ << ", pos=" << pos_ << std::endl;
            pos_ = len - pos;
            return;
        }
        pos += e;
    }
    ASSERT(from  == Offset(0) && estimate() == Length(0));
}


bool PartHandle::merge(DataHandle* other)
{
    if(other->isEmpty())
        return true;

    // Poor man's RTTI,
    // Does not support inheritance

    if( !sameClass(*other) )
        return false;

    // We should be safe to cast now....

    PartHandle* handle = dynamic_cast<PartHandle*>(other);

    if(handle_ != handle->handle_)
        return false;

    ASSERT(handle->offset_.size() == handle->length_.size());

    offset_.reserve(offset_.size() + handle->offset_.size());
    length_.reserve(length_.size() + handle->length_.size());

    for(Ordinal i = 0; i < handle->offset_.size() ; ++i)
    {
        offset_.push_back(handle->offset_[i]);
        length_.push_back(handle->length_[i]);
    }

    compress(false);
    return true;
}

Length PartHandle::estimate()
{
    return accumulate(length_.begin(),length_.end(),Length(0));
}



//-----------------------------------------------------------------------------

} // namespace eckit

