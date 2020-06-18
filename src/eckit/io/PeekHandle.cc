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
#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/PeekHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#if 0
ClassSpec PeekHandle::classSpec_ = {&DataHandle::classSpec(), "PeekHandle",};
Reanimator<PeekHandle> PeekHandle::reanimator_;
#endif


PeekHandle::PeekHandle(DataHandle* h) :
    HandleHolder(h) {}

PeekHandle::PeekHandle(DataHandle& h) :
    HandleHolder(h) {}

PeekHandle::~PeekHandle() {}

Length PeekHandle::openForRead() {
    peek_.clear();
    return handle().openForRead();
}

void PeekHandle::skip(const Length& len) {
    NOTIMP;
}

unsigned char PeekHandle::peek(size_t n) {
    while (n >= peek_.size()) {
        unsigned char c;
        ASSERT(handle().read(&c, 1) == 1);
        peek_.push_back(c);
    }
    return peek_[n];
}


long PeekHandle::peek(void* buffer, size_t size, size_t pos) {
    size_t last = pos + size;

    unsigned char* buf = static_cast<unsigned char*>(buffer);

    while (last > peek_.size()) {
        long n = std::min(last - peek_.size(), size);
        long p = handle().read(buf, n);

        ASSERT(p >= 0);
        if (p == 0) {
            break;
        }

        std::copy(buf,
                  buf + p,
                  std::back_inserter(peek_));

    }

    int len = std::min(last, peek_.size());
    ASSERT(len >= pos);
    len -= pos;

    std::copy(peek_.begin() + pos,
              peek_.begin() + pos + len,
              buf);

    return len;

}

long PeekHandle::read(void* buffer, long length) {

    if (peek_.empty()) {
        return handle().read(buffer, length);
    }

    long len = 0;
    char *p = static_cast<char*>(buffer);

    size_t s = std::min(peek_.size(), size_t(length));

    std::copy(peek_.begin(),
              peek_.begin() + s,
              p);

    p += s;
    len += s;
    length -= s;

    while (s--) {
        peek_.pop_front();
    }

    if (length) {

        int n = handle().read(p, length);
        ASSERT (n >= 0);
        len += n;
    }

    return len;
}

void PeekHandle::close() {
    peek_.clear();
    handle().close();
}

void PeekHandle::rewind() {
    peek_.clear();
    handle().rewind();
}

Offset PeekHandle::seek(const Offset& off) {
    peek_.clear();
    return handle().seek(off);
}

void PeekHandle::print(std::ostream& s) const {
    s << "PeekHandle[";
    handle().print(s);
    s << ']';
}

Length PeekHandle::estimate() {
    return handle().estimate();
}

bool PeekHandle::canSeek() const {
    return handle().canSeek();
}

Offset PeekHandle::position() {
    return static_cast<unsigned long long>(handle().position()) - peek_.size();
}

std::string PeekHandle::title() const {
    return std::string("{") + handle().title() + "}";
}

//----------------------------------------------------------------------------------------------------------------------


PeekWrapperHandle::PeekWrapperHandle(PeekHandle& handle) :
    handle_(handle) {}

PeekWrapperHandle::~PeekWrapperHandle() {}


void PeekWrapperHandle::skip(const Length& len) {
    position_ += len;
}

long PeekWrapperHandle::read(void* buffer, long length) {
    long len = handle_.peek(buffer, length, position_);
    ASSERT(len >= 0);
    position_ += len;
    return len;
}

void PeekWrapperHandle::rewind() {
    position_ = 0;
}

Offset PeekWrapperHandle::seek(const Offset& off) {
    position_ = off;
    return position_;
}

void PeekWrapperHandle::print(std::ostream& s) const {
    s << "PeekWrapperHandle[";
    handle_.print(s);
    s << ']';
}

bool PeekWrapperHandle::canSeek() const {
    return true;
}

Offset PeekWrapperHandle::position() {
    return position_;
}

//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
