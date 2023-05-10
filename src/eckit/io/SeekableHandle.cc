/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/SeekableHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SeekableHandle::SeekableHandle(PeekHandle* h) :
    owned_(true),
    handle_(h),
    seekableStart_(0),
    position_(0) {}

SeekableHandle::SeekableHandle(PeekHandle& h) :
    owned_(false),
    handle_(&h),
    seekableStart_(0),
    position_(0) {}

SeekableHandle::~SeekableHandle() {
    if (owned_) {
        delete handle_;
    }
}

Length SeekableHandle::openForRead() {
    position_      = 0;
    seekableStart_ = 0;
    return handle_->openForRead();
}

void SeekableHandle::close() {
    handle_->close();
}

void SeekableHandle::print(std::ostream& s) const {
    s << "SeekableHandle[";
    handle_->print(s);
    s << ']';
}

Length SeekableHandle::estimate() {
    return handle_->estimate();
}

void SeekableHandle::skip(const Length& len) {
    ASSERT(position_ + len <= seekableStart_ + Length(handle_->peeked()));
    position_ += len;
}

long SeekableHandle::read(void* buffer, long length) {
    long len = handle_->peek(buffer, length, position_ - seekableStart_);
    ASSERT(len >= 0);
    position_ += len;
    return len;
}

void SeekableHandle::rewind() {
    ASSERT(seekableStart_ == Offset(0));
    position_ = 0;
}

Offset SeekableHandle::seek(const Offset& off) {
    ASSERT(off >= seekableStart_);
    ASSERT(off <= seekableStart_ + Length(handle_->peeked()));
    position_ = off;
    return position_;
}

bool SeekableHandle::canSeek() const {
    return true;
}

Offset SeekableHandle::position() {
    return position_;
}

std::string SeekableHandle::title() const {
    return std::string("{") + handle_->title() + "}";
}

void SeekableHandle::collectMetrics(const std::string& what) const {
    handle_->collectMetrics(what);
}

void SeekableHandle::clear() {
    Length peeked = handle_->peeked();
    handle_->skip(peeked);
    seekableStart_ += peeked;
    position_ = seekableStart_;
}

//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
