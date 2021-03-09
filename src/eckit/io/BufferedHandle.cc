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
#include "eckit/io/BufferedHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

BufferedHandle::BufferedHandle(DataHandle* h, size_t size) :
    HandleHolder(h), buffer_(size), pos_(0), size_(size), used_(0), eof_(false), read_(false), position_(0) {}

BufferedHandle::BufferedHandle(DataHandle& h, size_t size) :
    HandleHolder(h), buffer_(size), pos_(0), size_(size), used_(0), eof_(false), read_(false), position_(0) {}

BufferedHandle::~BufferedHandle() {}

Length BufferedHandle::openForRead() {
    read_ = true;
    used_ = pos_ = 0;
    eof_         = false;
    position_    = 0;
    return handle().openForRead();
}

void BufferedHandle::openForWrite(const Length& length) {
    read_     = false;
    pos_      = 0;
    position_ = 0;
    handle().openForWrite(length);
}

void BufferedHandle::openForAppend(const Length&) {
    NOTIMP;
}

void BufferedHandle::skip(const Length& len) {
    ASSERT(read_);
    unsigned long long left = used_ - pos_;
    unsigned long long n    = len;

    if (n < left) {
        position_ += n;
        pos_ += n;
        return;
    }

    seek(position() + len);
}

long BufferedHandle::read(void* buffer, long length) {
    long len  = 0;
    long size = length;
    char* buf = (char*)buffer;

    ASSERT(read_);

    if (eof_)
        return -1;

    while (len < length && !eof_) {
        long left = used_ - pos_;
        ASSERT(left >= 0);

        if (left == 0 && !eof_) {
            used_ = handle().read(buffer_, size_);
            pos_  = 0;
            if (used_ <= 0) {
                eof_ = true;
                len  = len ? len : used_;
                if (len > 0)
                    position_ += len;
                if (len == 0)
                    return -1;
                return len;
            }
            left = used_;
        }

        char* p = buffer_;
        long s  = size < left ? size : left;
        ::memcpy(buf + len, p + pos_, s);
        len += s;
        ASSERT(len <= length);
        pos_ += s;
        ASSERT(pos_ <= used_);
        size -= s;
        ASSERT(size >= 0);
    }

    if (len > 0)
        position_ += len;
    return len;
}

long BufferedHandle::write(const void* buffer, long length) {
    long written = 0;

    ASSERT(!read_);

    while (length > 0) {
        long left = size_ - pos_;
        ASSERT(left > 0);

        size_t len = std::min(left, length);
        ASSERT(len > 0);

        char* p       = buffer_;
        const char* q = static_cast<const char*>(buffer);
        ::memcpy(p + pos_, q + written, len);
        pos_ += len;
        written += len;
        length -= len;

        ASSERT(length >= 0);

        ASSERT(pos_ <= size_);
        if (pos_ == size_) {
            bufferFlush();
        }
    }
    position_ += written;
    return written;
}

void BufferedHandle::close() {
    if (!read_)
        bufferFlush();
    handle().close();
}

void BufferedHandle::flush() {
    bufferFlush();
    handle().flush();
}

void BufferedHandle::rewind() {
    position_ = 0;
    used_ = pos_ = 0;
    eof_         = false;
    handle().rewind();
}

Offset BufferedHandle::seek(const Offset& off) {
    used_ = pos_ = 0;
    eof_         = false;
    position_    = handle().seek(off);
    return position_;
}


void BufferedHandle::print(std::ostream& s) const {
    s << "BufferedHandle[";
    handle().print(s);
    s << ']';
}

Length BufferedHandle::estimate() {
    return handle().estimate();
}

Offset BufferedHandle::position() {
    // ASSERT(read_);
    return position_;
}

void BufferedHandle::bufferFlush() {
    if (pos_) {
        long len = handle().write(buffer_, pos_);
        ASSERT((size_t)len == pos_);
        pos_ = 0;
    }
}

std::string BufferedHandle::title() const {
    return std::string("{") + handle().title() + "}";
}

void BufferedHandle::collectMetrics(const std::string& what) const {
    handle().collectMetrics(what);
}

DataHandle* BufferedHandle::clone() const {
    return new BufferedHandle(handle().clone(), buffer_.size());
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
