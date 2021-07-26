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
#include "eckit/io/SeekableHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


SeekableHandle::SeekableHandle(PeekHandle* h) :
    handle_(*h), position_(0) {}

SeekableHandle::SeekableHandle(PeekHandle& h) :
    handle_(h), position_(0) {}

SeekableHandle::~SeekableHandle() {}

Length SeekableHandle::openForRead() {
    position_ = 0;
    return handle_.openForRead();
}

void SeekableHandle::close() {
    handle_.close();
}

void SeekableHandle::print(std::ostream& s) const {
    s << "SeekableHandle[";
    handle_.print(s);
    s << ']';
}

Length SeekableHandle::estimate() {
    return handle_.estimate();
}

void SeekableHandle::skip(const Length& len) {
    position_ += len;
}

long SeekableHandle::read(void* buffer, long length) {
    long len = handle_.peek(buffer, length, position_);
    ASSERT(len >= 0);
    position_ += len;
    return len;
}

void SeekableHandle::rewind() {
    position_ = 0;
}

Offset SeekableHandle::seek(const Offset& off) {
    position_ = off;
    return position_;
}

bool SeekableHandle::canSeek() const {
    return true;
}

Offset SeekableHandle::position() {
    return position_;
}

std::string SeekableHandle::title() const {
    return std::string("{") + handle_.title() + "}";
}

void SeekableHandle::collectMetrics(const std::string& what) const {
    handle_.collectMetrics(what);
}

//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
