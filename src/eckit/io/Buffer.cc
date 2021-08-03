/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

static char* allocate(size_t size) {
    return new char[size];
}

static void deallocate(char* buffer) {
    delete[] buffer;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(size_t size) :
    buffer_{nullptr}, size_{size} {
    create();
}

Buffer::Buffer(const void* p, size_t len) :
    buffer_{nullptr}, size_{len} {
    create();
    copy(p, len);
}

Buffer::Buffer(const std::string& s) :
    buffer_{nullptr}, size_{s.length() + 1} {
    create();
    copy(s);
}

Buffer::Buffer(Buffer&& rhs) noexcept :
    buffer_{rhs.buffer_}, size_{rhs.size_} {
    rhs.buffer_ = nullptr;
    rhs.size_   = 0;
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }

    deallocate(buffer_);

    buffer_ = rhs.buffer_;
    size_   = rhs.size_;

    rhs.buffer_ = nullptr;
    rhs.size_   = 0;

    return *this;
}

Buffer::~Buffer() {
    destroy();
}

void Buffer::zero() {
    if (buffer_) {
        ::memset(buffer_, 0, size_);
    }
}

void Buffer::create() {
    buffer_ = allocate(size_);
}

void Buffer::destroy() {
    if (buffer_) {
        deallocate(buffer_);
        buffer_ = nullptr;
        size_   = 0;
    }
}

void Buffer::copy(const std::string& s) {
    ASSERT(buffer_);
    ::strncpy(buffer_, s.c_str(), std::min(size_, s.size() + 1));
}

void Buffer::copy(const void* p, size_t size, size_t pos) {
    ASSERT(buffer_ && size_ >= pos + size);
    if (size) {
        ::memcpy(buffer_ + pos, p, size);
    }
}

void Buffer::resize(size_t size, bool preserveData) {
    if (size != size_) {
        if (preserveData) {
            char* newbuffer = allocate(size);
            ::memcpy(newbuffer, buffer_, std::min(size_, size));
            deallocate(buffer_);
            size_   = size;
            buffer_ = newbuffer;
        }
        else {
            deallocate(buffer_);
            size_   = size;
            buffer_ = allocate(size);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
