/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File BufferCache.cc
// Baudouin Raoult - (c) ECMWF Jul 11


#include "eckit/io/BufferCache.h"
#include "eckit/serialisation/Stream.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

BufferCache::BufferCache(size_t size) :
    count_(0), buffer_(size), updated_(::time(0)) {}

BufferCache::BufferCache(const BufferCache& other) :
    count_(other.count_), buffer_(other.buffer_.size()), updated_(::time(0)) {
    ::memcpy((char*)buffer_, (const char*)other.buffer_, count_);
}

BufferCache::~BufferCache() {}

BufferCache& BufferCache::operator=(const BufferCache& other) {
    if (this != &other) {
        count_ = other.count_;
        buffer_.resize(other.buffer_.size());
        ::memcpy((char*)buffer_, (const char*)other.buffer_, count_);
        updated_ = ::time(0);
    }
    return *this;
}

bool BufferCache::operator<(const BufferCache& other) const {
    return (count_ < other.count_) || ((count_ == other.count_) && (::memcmp(buffer_, other.buffer_, count_) < 0));
}

void BufferCache::reset() {
    count_ = 0;
}

void BufferCache::add(const void* buffer, size_t len) {
    if (buffer_.size() < count_ + len) {
        buffer_.resize(count_ + len + 1024);
    }
    ::memcpy(((char*)buffer_) + count_, buffer, len);
    count_ += len;
}


void BufferCache::print(std::ostream& s) const {
    Stream::dump(s, buffer_, count_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
