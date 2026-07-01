/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosPartHandle.h"

#include "eckit/exception/Exceptions.h"

using eckit::Length;
using eckit::Offset;

namespace eckit {

RadosPartHandle::RadosPartHandle(const eckit::RadosObject& object, 
    const eckit::Offset& off,
    const eckit::Length& len) : object_(object), open_(false), offset_(off), len_(len) {}

RadosPartHandle::~RadosPartHandle() {

    if (open_) eckit::Log::error() << "RadosPartHandle not closed before destruction." << std::endl;

}

void RadosPartHandle::print(std::ostream& s) const {
    s << "RadosPartHandle[notimp]";
}

Length RadosPartHandle::openForRead() {

    ASSERT(!open_);

    open_ = true;

    return size();

}

long RadosPartHandle::read(void* buf, long len) {

    ASSERT(open_);

    eckit::Length s = size();
    if (len > s - offset_) len = s - offset_;

    int read = RADOS_CALL(
        rados_read(
            RadosCluster::instance().ioCtx(object_), 
            object_.name().c_str(),
            reinterpret_cast<char*>(buf),
            len, offset_
        )
    );

    offset_ += read;

    return read;

}

void RadosPartHandle::close() {

    if (!open_) return;

    open_ = false;

}

void RadosPartHandle::flush() {

    /// empty implmenetation

}

Length RadosPartHandle::size() {

    return len_;

}

Length RadosPartHandle::estimate() {

    return size();

}

Offset RadosPartHandle::position() {

    return offset_;

}

Offset RadosPartHandle::seek(const Offset& offset) {

    offset_ = offset;
    return offset_;

}

bool RadosPartHandle::canSeek() const {

    return true;

}

// void RadosPartHandle::skip(const Length& len) {

//     offset_ += Offset(len);

// }

std::string RadosPartHandle::title() const {
    
    return object_.name();

}

}  // namespace eckit
 