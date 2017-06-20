/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/io/CircularBuffer.h"
#include "eckit/memory/MemoryPool.h"
#include "eckit/maths/Functions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CircularBuffer::CircularBuffer(size_t size):
    size_(size),
    increment_(size),
    buffer_(new char[size]),
    pos_(0),
    used_(0)

{
    ASSERT(buffer_);
}


CircularBuffer::~CircularBuffer()
{
    delete[] buffer_;
}


size_t CircularBuffer::write(const void* buffer, size_t length) {
    size_t left = size_ - used_;

    if (length > left) {
        size_t newsize = eckit::round(size_ + length, increment_);
        char* buffer = new char[newsize];
        ASSERT(buffer);

        ASSERT(read(buffer, used_) == used_);

        pos_ = 0;
        size_ = newsize;
        delete[] buffer_;
        buffer_ = buffer;
    }


    const char* p = static_cast<const char*>(buffer);

    size_t start = (pos_ + used_) % size_;

    size_t n = std::min(size_ - start, size_t(length));
    ::memcpy(buffer_ + start, p, n);
    ::memcpy(buffer_, p + n, length - n);

    return length;

}

size_t CircularBuffer::read(void* buffer, size_t length) {
    size_t len = std::min(used_, length);

    char* p = static_cast<char*>(buffer);

    size_t n = size_ - pos_;
    ::memcpy(p, buffer_ + pos_, n);
    ::memcpy(p + n, buffer_, len - n);

    pos_ = (pos_ + len) % size_;
    used_ -= len;

    return len;
}

size_t CircularBuffer::length() const {
    return used_;
}

void CircularBuffer::clear() {
    pos_ = 0;
    used_ = 0;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

