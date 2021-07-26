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

#include "eckit/io/Buffer.h"
#include "eckit/maths/Functions.h"
#include "eckit/serialisation/ResizableMemoryStream.h"


namespace eckit {

ResizableMemoryStream::ResizableMemoryStream(Buffer& buffer) :
    buffer_(buffer), position_(0) {}

ResizableMemoryStream::~ResizableMemoryStream() {}

long ResizableMemoryStream::read(void* buffer, long length) {
    size_t left = buffer_.size() - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(buffer, buffer_ + position_, size);
    position_ += size;
    return long(size);
}

long ResizableMemoryStream::write(const void* buffer, long length) {
    size_t len  = size_t(length);
    size_t left = buffer_.size() - position_;

    if (left < len) {
        size_t reqsize = buffer_.size() + (len - left);
        size_t newsize = eckit::round(2 * reqsize, 64);  // grow in multiples of 2, rounded to 64
        //  eckit::Log::info() << "Resizing buffer from " << buffer_.size() << " to " << newsize << std::endl;
        buffer_.resize(newsize, true);  // preserves contents
        left = buffer_.size() - position_;
    }

    size_t written = std::min(left, size_t(length));
    ::memcpy(buffer_ + position_, buffer, written);
    position_ += written;

    return long(written);
}

void ResizableMemoryStream::rewind() {
    position_ = 0;
}

std::string ResizableMemoryStream::name() const {
    return "ResizableMemoryStream";
}

size_t ResizableMemoryStream::position() const {
    return position_;
}

}  // namespace eckit
