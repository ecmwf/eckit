/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/MultiPartHandle.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

MultiPartHandle::MultiPartHandle(DataHandle* handle, const Length& size, MultiPartHandle* prev) :
    handle_(handle),
    size_(size),
    prev_(prev),
    next_(nullptr),
    position_(0),
    start_(0),
    opened_(false),
    opens_(0) {
    if (prev_) {
        ASSERT(prev_->next_ == nullptr);
        prev_->next_ = this;
        start_       = prev_->start_ + prev_->size_;
    }
}

MultiPartHandle::~MultiPartHandle() {
    if (next_ == nullptr) {
        delete handle_;
    }
}

Length MultiPartHandle::openForRead() {
    ASSERT(!opened_);
    opened_ = true;

    first().openHandle();

    rewind();
    return estimate();
}

void MultiPartHandle::openForWrite(const Length&) {
    NOTIMP;
}

void MultiPartHandle::openForAppend(const Length&) {
    NOTIMP;
}


long MultiPartHandle::read(void* buffer, long length) {
    ASSERT(opened_);
    size_t left = size_ - position_;
    size_t size = std::min(left, size_t(length));

    size = handle_->read(buffer, size);
    position_ += size;

    return size;
}

long MultiPartHandle::write(const void*, long) {
    NOTIMP;
}

void MultiPartHandle::close() {
    ASSERT(opened_);
    opened_ = false;
    first().closeHandle();
}

void MultiPartHandle::flush() {
    NOTIMP;
}

void MultiPartHandle::rewind() {
    seek(0);
}

void MultiPartHandle::print(std::ostream& s) const {
    if (format(s) == Log::compactFormat)
        s << "MultiPartHandle";
    else {
        s << "MultiPartHandle[";
        // for (size_t i = 0; i < datahandles_.size(); i++)
        // {
        //     if (i != 0)
        //         s << ",(";
        // handle_->print(s);
        //     s << ")";
        // }
        s << handle_;
        s << ']';
    }
}

bool MultiPartHandle::merge(DataHandle*) {
    return false;
}

Length MultiPartHandle::estimate() {
    return size_;
}

void MultiPartHandle::restartReadFrom(const Offset& from) {
    Log::warning() << *this << " restart read from " << from << std::endl;
    ASSERT(opened_);
    seek(from);
}

void MultiPartHandle::toRemote(Stream&) const {
    NOTIMP;
}

void MultiPartHandle::toLocal(Stream&) const {
    NOTIMP;
}

DataHandle* MultiPartHandle::toLocal() {
    NOTIMP;
}

void MultiPartHandle::cost(std::map<std::string, Length>&, bool) const {
    NOTIMP;
}

bool MultiPartHandle::moveable() const {
    return false;
}

Offset MultiPartHandle::position() {
    return position_;
}

Offset MultiPartHandle::seek(const Offset& offset) {
    position_ = offset;
    if (position_ > size_) {
        position_ = (unsigned long long)size_;
    }

    ASSERT(handle_->seek(start_ + position_) == Offset(start_ + position_));
    return position_;
}

void MultiPartHandle::skip(const Length& length) {
    seek(position_ + length);
}


std::string MultiPartHandle::title() const {
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

bool MultiPartHandle::compress(bool) {
    return false;
}


//-----------------------------------------------------------------------------

MultiPartHandle& MultiPartHandle::first() {
    MultiPartHandle* h = this;
    while (h->prev_) {
        h = h->prev_;
    }
    return *h;
}

void MultiPartHandle::openHandle() {
    ASSERT(prev_ == nullptr);
    if (opens_ == 0) {
        handle_->openForRead();
    }
    opens_++;
}

void MultiPartHandle::closeHandle() {
    ASSERT(prev_ == nullptr);
    ASSERT(opens_ > 0);
    opens_--;
    if (opens_ == 0) {
        handle_->close();
    }
}


}  // namespace eckit
