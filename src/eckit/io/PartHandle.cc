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

#include "eckit/config/Resource.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"

#include "eckit/io/PartFileHandle.h"
#include "eckit/io/PartHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec PartHandle::classSpec_ = {
    &DataHandle::classSpec(),
    "PartHandle",
};
Reanimator<PartHandle> PartHandle::reanimator_;

void PartHandle::print(std::ostream& s) const {
    /*
    if(format(s) == Log::compactFormat)
        s << "PartHandle";
    else*/
    s << "PartHandle[handle=" << handle() << ",offset=" << offset_ << ",length=" << length_ << ']';
}

void PartHandle::encode(Stream& s) const {
    DataHandle::encode(s);
    s << handle();
    s << offset_;
    s << length_;
}

PartHandle::PartHandle(Stream& s) :
    DataHandle(s), HandleHolder(Reanimator<DataHandle>::reanimate(s)), pos_(0), index_(0) {
    s >> offset_;
    s >> length_;

    ASSERT(offset_.size() == length_.size());
}


PartHandle::PartHandle(DataHandle& handle, const OffsetList& offset, const LengthList& length) :
    HandleHolder(handle), pos_(0), index_(0), offset_(offset), length_(length) {
    ASSERT(offset_.size() == length_.size());
}

PartHandle::PartHandle(DataHandle* handle, const OffsetList& offset, const LengthList& length) :
    HandleHolder(handle), pos_(0), index_(0), offset_(offset), length_(length) {
    ASSERT(offset_.size() == length_.size());
}

PartHandle::PartHandle(DataHandle& handle, const Offset& offset, const Length& length) :
    HandleHolder(handle), pos_(0), index_(0), offset_(1, offset), length_(1, length) {
    ASSERT(offset_.size() == length_.size());
}

PartHandle::PartHandle(DataHandle* handle, const Offset& offset, const Length& length) :
    HandleHolder(handle), pos_(0), index_(0), offset_(1, offset), length_(1, length) {
    ASSERT(offset_.size() == length_.size());
}

PartHandle::~PartHandle() {}

Length PartHandle::openForRead() {
    handle().openForRead();
    rewind();
    return estimate();
}

void PartHandle::openForWrite(const Length&) {
    NOTIMP;
}

void PartHandle::openForAppend(const Length&) {
    NOTIMP;
}

long PartHandle::read1(char* buffer, long length) {
    // skip empty entries if any
    while (index_ < offset_.size() && length_[index_] == Length(0))
        index_++;

    ASSERT(index_ <= offset_.size());

    if (index_ == offset_.size())
        return 0;


    Length ll = (long long)offset_[index_] + Length(pos_);
    off_t pos = ll;


    ASSERT(handle().seek(pos) == Offset(pos));


    ll           = length_[index_] - Length(pos_);
    Length lsize = std::min(Length(length), ll);
    long size    = lsize;

    ASSERT(Length(size) == lsize);

    long n = handle().read(buffer, size);

    if (n != size) {
        std::ostringstream s;
        s << handle() << ": cannot read " << size << ", got only " << n;
        throw ReadError(s.str());
    }

    pos_ += n;
    if (pos_ >= length_[index_]) {
        index_++;
        pos_ = 0;
    }

    return n;
}


long PartHandle::read(void* buffer, long length) {
    char* p = static_cast<char*>(buffer);

    long n     = 0;
    long total = 0;

    while (length > 0 && (n = read1(p, length)) > 0) {
        length -= n;
        total += n;
        p += n;
    }

    return total > 0 ? total : n;
}

long PartHandle::write(const void*, long) {
    return -1;
}

void PartHandle::close() {
    handle().close();
}

void PartHandle::rewind() {
    pos_   = 0;
    index_ = 0;
}

void PartHandle::restartReadFrom(const Offset& from) {
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


Length PartHandle::estimate() {
    return std::accumulate(length_.begin(), length_.end(), Length(0));
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
