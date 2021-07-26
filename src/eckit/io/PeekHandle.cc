/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/PeekHandle.h"
#include "eckit/log/Bytes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

PeekHandle::PeekHandle(DataHandle* h) :
    HandleHolder(h) {}

PeekHandle::PeekHandle(DataHandle& h) :
    HandleHolder(h) {}

PeekHandle::~PeekHandle() {}

Length PeekHandle::openForRead() {
    peek_.clear();
    return handle().openForRead();
}

void PeekHandle::skip(const Length&) {
    NOTIMP;
}

unsigned char PeekHandle::peek(size_t n) {
    while (n >= peek_.size()) {
        unsigned char c;
        if (handle().read(&c, 1) != 1) {
            std::ostringstream s;
            s << handle() << ": failed to read 1 byte";
            throw ReadError(s.str(), Here());
        }
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

        if (p < 0) {
            std::ostringstream s;
            s << handle() << ": failed to read " << Bytes(n);
            throw ReadError(s.str(), Here());
        }
        if (p == 0) {
            break;
        }

        std::copy(buf, buf + p, std::back_inserter(peek_));
    }

    int len = std::min(last, peek_.size());
    ASSERT(len >= pos);
    len -= pos;

    std::copy(peek_.begin() + pos, peek_.begin() + pos + len, buf);

    return len;
}

long PeekHandle::read(void* buffer, long length) {

    if (peek_.empty()) {
        return handle().read(buffer, length);
    }

    long len = 0;
    char* p  = static_cast<char*>(buffer);

    size_t s = std::min(peek_.size(), size_t(length));

    std::copy(peek_.begin(), peek_.begin() + s, p);

    p += s;
    len += s;
    length -= s;

    while (s--) {
        peek_.pop_front();
    }

    if (length) {

        int n = handle().read(p, length);
        if (n < 0) {
            std::ostringstream s;
            s << handle() << ": failed to read " << Bytes(length);
            throw ReadError(s.str(), Here());
        }
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

Length PeekHandle::size() {
    return handle().size();
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

void PeekHandle::collectMetrics(const std::string& what) const {
    handle().collectMetrics(what);
}

size_t PeekHandle::peeked() const {
    return peek_.size();
}


//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
