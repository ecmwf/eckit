/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <numeric>

#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Log.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/PartFileHandle.h"

#include "eckit/io/PooledHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec PartFileHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "PartFileHandle",
};
Reanimator<PartFileHandle> PartFileHandle::reanimator_;

void PartFileHandle::print(std::ostream& s) const {
    if (format(s) == Log::compactFormat)
        s << "PartFileHandle";
    else
        s << "PartFileHandle[path=" << path_ << ",offset=" << offset_ << ",length=" << length_ << ']';
}

void PartFileHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << path_;
    s << offset_;
    s << length_;
}

PartFileHandle::PartFileHandle(Stream& s) :
    DataHandle(s), pos_(0), index_(0) {
    s >> path_;
    s >> offset_;
    s >> length_;

    ASSERT(offset_.size() == length_.size());
}

PartFileHandle::PartFileHandle(const PathName& name, const OffsetList& offset, const LengthList& length) :
    path_(name), handle_(), pos_(0), index_(0), offset_(offset), length_(length) {
    //    Log::info() << "PartFileHandle::PartFileHandle " << name << std::endl;
    ASSERT(offset_.size() == length_.size());
    compress(false);
}

PartFileHandle::PartFileHandle(const PathName& name, const Offset& offset, const Length& length) :
    path_(name), handle_(), pos_(0), index_(0), offset_(1, offset), length_(1, length) {}


DataHandle* PartFileHandle::clone() const {
    return new PartFileHandle(path_, offset_, length_);
}


bool PartFileHandle::compress(bool sorted) {
    if (sorted)
        eckit::sort(offset_, length_);
    return eckit::compress(offset_, length_);
}

PartFileHandle::~PartFileHandle() {}

Length PartFileHandle::openForRead() {
    if (!handle_) {
        // The handle may already exists if a  restartReadFrom()
        // is requested
        handle_.reset(new PooledHandle(path_));
    }
    handle_->openForRead();
    rewind();
    return estimate();
}

void PartFileHandle::openForWrite(const Length&) {
    NOTIMP;
}

void PartFileHandle::openForAppend(const Length&) {
    NOTIMP;
}

long PartFileHandle::read1(char* buffer, long length) {
    ASSERT(handle_);

    // skip empty entries if any
    while (index_ < offset_.size() && length_[index_] == Length(0))
        index_++;

    if (index_ == offset_.size())
        return 0;


    Length ll = (long long)offset_[index_] + Length(pos_);
    off_t pos = ll;

    handle_->seek(pos);

    ll           = length_[index_] - Length(pos_);
    Length lsize = std::min(Length(length), ll);
    long size    = lsize;

    ASSERT(Length(size) == lsize);

    long n = handle_->read(buffer, size);

    if (n != size) {
        std::ostringstream s;
        s << path_ << ": cannot read " << size << ", got only " << n;
        throw ReadError(s.str());
    }

    pos_ += n;
    if (pos_ >= length_[index_]) {
        index_++;
        pos_ = 0;
    }

    return n;
}


long PartFileHandle::read(void* buffer, long length) {
    char* p = (char*)buffer;

    long n     = 0;
    long total = 0;

    while (length > 0 && (n = read1(p, length)) > 0) {
        length -= n;
        total += n;
        p += n;
    }

    return total > 0 ? total : n;
}

long PartFileHandle::write(const void*, long) {
    NOTIMP;
}

void PartFileHandle::close() {
    if (handle_) {
        handle_->close();
        // Don't delete the handle here so the PooledHandle entry continues
        // to live, so the underlying file is not closed, which give a chance to PooledHandle
        // to do its work, for example of that PartFileHandle is itself part of
        // a multihandle that points many time to the same path
        // handle_.reset();
    }
}

void PartFileHandle::rewind() {
    pos_   = 0;
    index_ = 0;
}

void PartFileHandle::restartReadFrom(const Offset& from) {
    Log::warning() << *this << " restart read from " << from << std::endl;
    rewind();
    long long len = from;
    long long pos = 0;

    for (index_ = 0; index_ < length_.size(); index_++) {
        long long e = length_[index_];
        if (len >= pos && len < pos + e) {
            Log::warning() << *this << " restart read from " << from << ", index=" << index_ << ", pos=" << pos_
                           << std::endl;
            pos_ = len - pos;
            return;
        }
        pos += e;
    }
    ASSERT(from == Offset(0) && estimate() == Length(0));
}

Offset PartFileHandle::position() {
    long long position = 0;
    for (Ordinal i = 0; i < index_; i++) {
        position += length_[i];
    }
    return position + Length(pos_);
}

Offset PartFileHandle::seek(const Offset& offset) {
    rewind();
    const long long seekto = offset;
    long long accumulated  = 0;

    for (index_ = 0; index_ < length_.size(); index_++) {
        long long len = length_[index_];
        if (accumulated <= seekto && seekto < accumulated + len) {
            pos_ = seekto - accumulated;
            return offset;
        }
        accumulated += len;
    }
    long long beyond = seekto - accumulated;
    // check if seek went beyond EOF which is POSIX compliant, but we ASSERT so we find possible bugs
    ASSERT(not beyond);
    pos_ = 0;
    return seekto;
}

bool PartFileHandle::canSeek() const {
    return true;
}

bool PartFileHandle::merge(DataHandle* other) {
    if (other->isEmpty())
        return true;

    // Poor man's RTTI,
    // Does not support inheritance

    if (!sameClass(*other))
        return false;

    // We should be safe to cast now....

    PartFileHandle* handle = dynamic_cast<PartFileHandle*>(other);

    if (path_ != handle->path_)
        return false;

    ASSERT(handle->offset_.size() == handle->length_.size());

    offset_.reserve(offset_.size() + handle->offset_.size());
    length_.reserve(length_.size() + handle->length_.size());

    for (Ordinal i = 0; i < handle->offset_.size(); ++i) {
        offset_.push_back(handle->offset_[i]);
        length_.push_back(handle->length_[i]);
    }

    compress(false);
    return true;
}

Length PartFileHandle::size() {
    return std::accumulate(length_.begin(), length_.end(), Length(0));
}

Length PartFileHandle::estimate() {
    return std::accumulate(length_.begin(), length_.end(), Length(0));
}

void PartFileHandle::cost(std::map<std::string, Length>& c, bool read) const {
    if (read) {
        c[path_.node()] += const_cast<PartFileHandle*>(this)->estimate();
    }
}


std::string PartFileHandle::title() const {
    std::ostringstream os;
    os << PathName::shorten(path_) << " (" << length_.size() << ")";
    return os.str();
}

std::string PartFileHandle::metricsTag() const {
    return PathName::metricsTag(path_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
