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
#include "eckit/serialisation/MemoryStream.h"


namespace eckit {

MemoryStream::MemoryStream(const Buffer& buffer) :
    address_(const_cast<Buffer&>(buffer)), size_(buffer.size()), position_(0) {}


MemoryStream::MemoryStream(Buffer& buffer) :
    address_(buffer), size_(buffer.size()), position_(0) {}

MemoryStream::MemoryStream(const void* address, size_t size) :
    address_(const_cast<char*>(reinterpret_cast<const char*>(address))), size_(size), position_(0) {}


MemoryStream::MemoryStream(void* address, size_t size) :
    address_(reinterpret_cast<char*>(address)), size_(size), position_(0) {}

MemoryStream::~MemoryStream() {}

long MemoryStream::read(void* buffer, long length) {
    size_t left = size_ - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(buffer, address_ + position_, size);
    position_ += size;

    return long(size);
}

long MemoryStream::write(const void* buffer, long length) {
    size_t left = size_ - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(address_ + position_, buffer, size);
    position_ += size;

    return long(size);
}

void MemoryStream::rewind() {
    position_ = 0;
}

std::string MemoryStream::name() const {
    return "MemoryStream";
}

size_t MemoryStream::position() const {
    return position_;
}

}  // namespace eckit
