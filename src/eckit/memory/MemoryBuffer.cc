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
#include "eckit/memory/MemoryBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MemoryBuffer::MemoryBuffer(size_t size) :
    buffer_(0), size_(size) {
    create();
}

MemoryBuffer::MemoryBuffer(const char* p, size_t size) :
    buffer_(0), size_(size) {
    create();
    copy(p, size);
}

MemoryBuffer::MemoryBuffer(const std::string& s) :
    buffer_(0), size_(s.length() + 1) {
    create();
    copy(s);
}


MemoryBuffer::~MemoryBuffer() {
    destroy();
}

void MemoryBuffer::create() {
    buffer_ = new char[size_];
    ASSERT(buffer_);
}

void MemoryBuffer::destroy() {
    char* p = static_cast<char*>(buffer_);
    delete[] p;
}

void MemoryBuffer::copy(const std::string& s) {
    ::strcpy(static_cast<char*>(buffer_), s.c_str());
}

void MemoryBuffer::copy(const char* p, size_t size) {
    ::memcpy(buffer_, p, size);
}

void MemoryBuffer::swap(MemoryBuffer& rhs) {
    std::swap(buffer_, rhs.buffer_);
    std::swap(size_, rhs.size_);
}

void eckit::MemoryBuffer::resize(size_t size) {
    destroy();
    size_ = size;
    create();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
