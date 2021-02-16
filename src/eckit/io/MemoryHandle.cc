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
#include "eckit/io/MemoryHandle.h"
#include "eckit/maths/Functions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MemoryHandle::MemoryHandle(const Buffer& buffer) :
    address_(const_cast<Buffer&>(buffer)),
    size_(buffer.size()),
    capacity_(buffer.size()),
    opened_(false),
    readOnly_(true),
    read_(false),
    grow_(false),
    owned_(false),
    position_(0) {}


MemoryHandle::MemoryHandle(Buffer& buffer) :
    address_(buffer),
    size_(buffer.size()),
    capacity_(buffer.size()),
    opened_(false),
    readOnly_(false),
    read_(false),
    grow_(false),
    owned_(false),
    position_(0) {}

MemoryHandle::MemoryHandle(const void* address, size_t size) :
    address_(const_cast<char*>(reinterpret_cast<const char*>(address))),
    size_(size),
    capacity_(size),
    opened_(false),
    readOnly_(true),
    read_(false),
    grow_(false),
    owned_(false),
    position_(0) {}


MemoryHandle::MemoryHandle(void* address, size_t size) :
    address_(reinterpret_cast<char*>(address)),
    size_(size),
    capacity_(size),
    opened_(false),
    readOnly_(false),
    read_(false),
    grow_(false),
    owned_(false),
    position_(0) {}


MemoryHandle::MemoryHandle(size_t size, bool grow) :
    address_(nullptr),
    size_(0),
    capacity_(size),
    opened_(false),
    readOnly_(false),
    read_(false),
    grow_(grow),
    owned_(true),
    position_(0) {
    address_ = new char[capacity_];
    ASSERT(address_);
}

MemoryHandle::~MemoryHandle() {
    if (owned_) {
        delete[] address_;
    }
}

Length MemoryHandle::openForRead() {
    read_     = true;
    position_ = 0;
    opened_   = true;
    return size_;
}

void MemoryHandle::openForWrite(const Length&) {
    ASSERT(!readOnly_);
    read_     = false;
    position_ = 0;
    opened_   = true;
}

void MemoryHandle::openForAppend(const Length&) {
    NOTIMP;
}

void MemoryHandle::skip(const Length& len) {
    ASSERT(read_);
    seek(position() + len);
}

long MemoryHandle::read(void* buffer, long length) {
    ASSERT(opened_);
    ASSERT(read_);
    ASSERT(length >= 0);

    size_t left = size_ - position_;
    size_t size = std::min(left, size_t(length));
    ::memcpy(buffer, address_ + position_, size);
    position_ += size;

    return size;
}

long MemoryHandle::write(const void* buffer, long length) {
    ASSERT(opened_);
    ASSERT(!read_);
    ASSERT(length >= 0);

    size_t left = size_ - position_;


    if (grow_ && (left < size_t(length))) {
        if ((capacity_ - position_) < size_t(length)) {
            size_t newcapacity = round(capacity_ + size_t(length), 1024 * 1024);
            char* newdata      = new char[newcapacity];
            ASSERT(newdata);

            ::memcpy(newdata, address_, position_);
            delete[] address_;
            address_  = newdata;
            capacity_ = newcapacity;
        }

        size_ += length;

        left = size_ - position_;
    }

    size_t size = std::min(left, size_t(length));
    ::memcpy(address_ + position_, buffer, size);
    position_ += size;

    return size;
}

void MemoryHandle::close() {
    opened_ = false;
}

void MemoryHandle::flush() {}

void MemoryHandle::rewind() {
    ASSERT(opened_);
    position_ = 0;
}

Offset MemoryHandle::seek(const Offset& off) {
    ASSERT(opened_);
    ASSERT(size_t(off) <= size_);
    position_ = off;
    return position_;
}


void MemoryHandle::print(std::ostream& s) const {
    s << "MemoryHandle[size=" << size_ << ']';
}

Length MemoryHandle::estimate() {
    return size_;
}

Offset MemoryHandle::position() {
    ASSERT(opened_);
    return position_;
}

std::string MemoryHandle::title() const {
    return "<mem>";
}

DataHandle* MemoryHandle::clone() const {
    if (owned_) {
        MemoryHandle* h = new MemoryHandle(size_, grow_);
        ::memcpy(h->address_, address_, size_);
        h->size_ = size_;
        return h;
    }
    else {
        if (readOnly_) {
            return new MemoryHandle(static_cast<const void*>(address_), size_);
        }
        else {
            return new MemoryHandle(address_, size_);
        }
    }
}

const void* MemoryHandle::data() const {
    return address_;
}

Length MemoryHandle::size() {
    return size_;
}

std::string MemoryHandle::str() const {
    return std::string(address_, address_ + position_);
}

Length MemoryHandle::size() const {
    return size_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
