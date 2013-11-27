/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eckit/compat/StrStream.h"

#include "eckit/io/MarsFSPartHandle.h"
#include "eckit/io/PartFileHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


ClassSpec MarsFSPartHandle::classSpec_ = {&DataHandle::classSpec(),"MarsFSPartHandle",};
Reanimator<MarsFSPartHandle> MarsFSPartHandle::reanimator_;

void MarsFSPartHandle::print(std::ostream& s) const
{
    if(format(s) == Log::compactFormat)
        s << "MarsFSPartHandle";
    else
        s << "MarsFSPartHandle[path=" << path_
           << ",offset=" << offset_
           << ",length=" << length_ << ']';
}

void MarsFSPartHandle::encode(Stream& s) const
{
    DataHandle::encode(s);
    s << path_;
    s << offset_;
    s << length_;

}

MarsFSPartHandle::MarsFSPartHandle(Stream& s):
    DataHandle(s),
    path_(s)
{
    s >> offset_;
    s >> length_;

    ASSERT(offset_.size() == length_.size());
}

MarsFSPartHandle::MarsFSPartHandle(const MarsFSPath& path,
    const OffsetList& offset,const LengthList& length):
    path_(path),
    offset_(offset),
    length_(length)
{
    ASSERT(offset_.size() == length_.size());
    compress(false);
}

MarsFSPartHandle::MarsFSPartHandle(const MarsFSPath& path,
    const Offset& offset,const Length& length):
    path_(path),
    offset_(1,offset),
    length_(1,length)
{
}

bool MarsFSPartHandle::compress(bool sorted)
{
    if(sorted)
        eckit::sort(offset_,length_);
    return eckit::compress(offset_,length_);
}

MarsFSPartHandle::~MarsFSPartHandle()
{
}

Length MarsFSPartHandle::openForRead()
{
    ASSERT(!file_.get());
    file_ = std::auto_ptr<MarsFSFile>(new MarsFSFile(path_));
    file_->open("r");
    rewind();
    return estimate();
}

void MarsFSPartHandle::openForWrite(const Length&)
{
    NOTIMP;
}

void MarsFSPartHandle::openForAppend(const Length&)
{
    NOTIMP;
}

long MarsFSPartHandle::read1(char *buffer,long length)
{
    ASSERT(file_.get());
    // skip empty entries if any
    while (index_ < offset_.size() && length_[index_] == Length(0))
        index_++;

    if(index_ == offset_.size()) return 0;


    Offset pos  = (long long)offset_[index_] + Length(pos_);
    ASSERT(file_->seek(pos) == pos);

    Length ll    = length_[index_] - Length(pos_);
    Length lsize = std::min(Length(length),ll);
    long size  = lsize;

    ASSERT(Length(size) == lsize);

    long n = file_->read(buffer,size);

    if(n != size)
    {
        StrStream s;
        s << path_ << ": cannot read " << size << ", got only " << n << StrStream::ends;
        throw ReadError(string(s));
    }

    pos_ += n;
    if(pos_ >= length_[index_])
    {
        index_++;
        pos_ = 0;
    }

    return n;
}


long MarsFSPartHandle::read(void* buffer,long length)
{
    char *p = (char*)buffer;

    long n = 0;
    long total = 0;

    //Log::info() << "MarsFSPartHandle::read " << length << std::endl;

    while( length > 0 && (n = read1(p,length)) >0)
    {
        length -= n;
        total  += n;
        p      += n;
    }

    return total>0?total:n;

}

long MarsFSPartHandle::write(const void* buffer,long length)
{
    return -1;
}

void MarsFSPartHandle::close()
{
    if(file_.get()) {
        file_->close();
        file_ = std::auto_ptr<MarsFSFile>(0);
    }
}

void MarsFSPartHandle::rewind()
{
    pos_   = 0;
    index_ = 0;
}

void MarsFSPartHandle::restartReadFrom(const Offset& from)
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


bool MarsFSPartHandle::merge(DataHandle* other)
{
    if(other->isEmpty())
        return true;

    // Poor man's RTTI,
    // Does not support inheritance

    if( !sameClass(*other) )
        return false;

    // We should be safe to cast now....

    MarsFSPartHandle* handle = dynamic_cast<MarsFSPartHandle*>(other);

    if(path_ != handle->path_)
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

Length MarsFSPartHandle::estimate()
{
    return accumulate(length_.begin(),length_.end(),Length(0));
}

void MarsFSPartHandle::toLocal(Stream& s) const
{
    // TODO: Return PathFileHandle if local
    if(path_.isLocal())
    {
        PartFileHandle ph(path_.path(), offset_, length_);
        s << ph;
        return;
    }
    s << *this;
}

DataHandle* MarsFSPartHandle::toLocal()
{
    // TODO: Return PathFileHandle if local
    if(path_.isLocal())
    {
        return new PartFileHandle(path_.path(), offset_, length_);
    }
    return this;
}

void MarsFSPartHandle::cost(map<string,Length>& c, bool read) const
{
    if(read) {
        c[path_.node()] += const_cast<MarsFSPartHandle*>(this)->estimate();
    }
}

string MarsFSPartHandle::title() const
{
    StrStream os;
    os << "marsfs:/" << path_.node() << "/" << PathName::shorten(path_.path())
        << " (" << length_.size() << ")" << StrStream::ends;
    return string(os);
}

//-----------------------------------------------------------------------------

} // namespace eckit

