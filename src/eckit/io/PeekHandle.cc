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

long PeekHandle::read(void* buffer, long length) {

    if (peek_.empty()) {
        return handle().read(buffer, length);
    }

    long len = 0;
    char *p = static_cast<char*>(buffer);

    size_t s = std::min(peek_.size(), size_t(length));

    ::memcpy(p, &peek_[0], s);
    p += s;
    len += s;
    length -= s;

    while (s--) {
        peek_.erase(peek_.begin());
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

DataHandle* PeekHandle::clone() const {
    return new PeekHandle(handle().clone());
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
