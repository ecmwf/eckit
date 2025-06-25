/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"

#include <algorithm>
#include <cstring>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

char* allocate(size_t size) {
    return size == 0 ? nullptr : new char[size];
}

void deallocate(char*& buffer) {
    delete[] buffer;
    buffer = nullptr;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Buffer::Buffer(size_t size) : size_{size} {
    create();
}

Buffer::Buffer(const void* p, size_t size) : size_{size} {
    create();
    copy(p, size);
}

Buffer::Buffer(const std::string& s) : size_{s.length() + 1} {
    create();
    copy(s);
}

Buffer::Buffer(Buffer&& rhs) noexcept : buffer_{rhs.buffer_}, size_{rhs.size_} {
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
    if (buffer_ != nullptr) {
        std::memset(buffer_, 0, size_);
    }
}

void Buffer::create() {
    buffer_ = allocate(size_);
}

void Buffer::destroy() {
    deallocate(buffer_);
    size_ = 0;
}

void Buffer::copy(const std::string& s) {
    if (buffer_ != nullptr) {
        std::strncpy(buffer_, s.c_str(), std::min(size_, s.size() + 1));
    }
}

void Buffer::copy(const void* p, size_t size, size_t pos) {
    ASSERT(size_ >= pos + size);
    if (buffer_ != nullptr && size > 0) {
        std::memcpy(buffer_ + pos, p, size);
    }
}

void Buffer::resize(size_t size, bool preserveData) {
    if (size != size_) {
        if (preserveData) {
            auto* newbuffer = allocate(size);
            std::memcpy(newbuffer, buffer_, std::min(size_, size));
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
