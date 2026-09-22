// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File BufferCache.cc
// Baudouin Raoult - (c) ECMWF Jul 11

#include "eckit/io/BufferCache.h"

#include "eckit/serialisation/Stream.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

BufferCache::BufferCache(size_t size) : count_(0), buffer_(size), updated_(::time(nullptr)) {}

BufferCache::BufferCache(const BufferCache& other) :
    count_(other.count_), buffer_(other.buffer_.size()), updated_(::time(nullptr)) {
    ::memcpy((char*)buffer_, (const char*)other.buffer_, count_);
}

BufferCache::~BufferCache() {}

BufferCache& BufferCache::operator=(const BufferCache& other) {
    if (this != &other) {
        count_ = other.count_;
        buffer_.resize(other.buffer_.size());
        ::memcpy((char*)buffer_, (const char*)other.buffer_, count_);
        updated_ = ::time(nullptr);
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
