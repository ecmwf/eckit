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

#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Log.h"

#include "eckit/io/MarsFSPartHandle.h"
#include "eckit/io/PartFileHandle.h"


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
        s << "PartFileHandle[path=" << name_ << ",offset=" << offset_ << ",length=" << length_ << ']';
}

void PartFileHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << name_;
    s << offset_;
    s << length_;
}

PartFileHandle::PartFileHandle(Stream& s) : DataHandle(s), pos_(0), index_(0) {
    s >> name_;
    s >> offset_;
    s >> length_;

    ASSERT(offset_.size() == length_.size());
}

PartFileHandle::PartFileHandle(const PathName& name, const OffsetList& offset, const LengthList& length) :
    name_(name),
    file_(),
    pos_(0),
    index_(0),
    offset_(offset),
    length_(length) {
    //    Log::info() << "PartFileHandle::PartFileHandle " << name << std::endl;
    ASSERT(offset_.size() == length_.size());
    compress(false);
}

PartFileHandle::PartFileHandle(const PathName& name, const Offset& offset, const Length& length) :
    name_(name),
    file_(),
    pos_(0),
    index_(0),
    offset_(1, offset),
    length_(1, length) {}


DataHandle* PartFileHandle::clone() const {
    return new PartFileHandle(name_, offset_, length_);
}


bool PartFileHandle::compress(bool sorted) {
    if (sorted)
        eckit::sort(offset_, length_);
    return eckit::compress(offset_, length_);
}

PartFileHandle::~PartFileHandle() {
    close();
}

Length PartFileHandle::openForRead() {
    ASSERT(!file_);
    file_.reset(new PooledFile(name_));
    file_->open();
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
    ASSERT(file_);

    // skip empty entries if any
    while (index_ < offset_.size() && length_[index_] == Length(0))
        index_++;

    if (index_ == offset_.size())
        return 0;


    Length ll = (long long)offset_[index_] + Length(pos_);
    off_t pos = ll;

    file_->seek(pos);

    ll           = length_[index_] - Length(pos_);
    Length lsize = std::min(Length(length), ll);
    long size    = lsize;

    ASSERT(Length(size) == lsize);

    long n = file_->read(buffer, size);

    if (n != size) {
        std::ostringstream s;
        s << name_ << ": cannot read " << size << ", got only " << n;
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
    if (file_) {
        file_->close();
        file_.reset();
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

Offset PartFileHandle::seek(const Offset& from) {
    rewind();
    long long len = from;
    long long pos = 0;

    for (index_ = 0; index_ < length_.size(); index_++) {
        long long e = length_[index_];
        if (len >= pos && len < pos + e) {
            pos_ = len - pos;
            return from;
        }
        pos += e;
    }
    return pos;
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

    if (name_ != handle->name_)
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

Length PartFileHandle::estimate() {
    return std::accumulate(length_.begin(), length_.end(), Length(0));
}

void PartFileHandle::toRemote(Stream& s) const {
    MarsFSPath p(PathName(name_).clusterName());
    MarsFSPartHandle remote(p, offset_, length_);
    s << remote;
}

void PartFileHandle::cost(std::map<std::string, Length>& c, bool read) const {
    if (read) {
        c[NodeInfo::thisNode().node()] += const_cast<PartFileHandle*>(this)->estimate();
    }
}


std::string PartFileHandle::title() const {
    std::ostringstream os;
    os << PathName::shorten(name_) << " (" << length_.size() << ")";
    return os.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
