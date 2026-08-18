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

#include <rados/librados.h>

#include <algorithm>
#include <ostream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosPartHandle::RadosPartHandle(const eckit::RadosObject& object, const eckit::Offset& off, const eckit::Length& len) :
    object_(object), open_(false), offset_(off), len_(len), pos_(0) {}

RadosPartHandle::~RadosPartHandle() {
    if (open_) {
        eckit::Log::error() << "RadosPartHandle not closed before destruction." << std::endl;
    }
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

    const long long remaining = len_ - pos_;

    if (remaining <= 0) {
        return 0;
    }

    len = std::min<long>(len, remaining);

    int read = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                     reinterpret_cast<char*>(buf), len, offset_ + pos_));

    pos_ += eckit::Length(read);

    return read;
}

void RadosPartHandle::close() {

    if (!open_) {
        return;
    }

    open_ = false;
}

void RadosPartHandle::flush() {}

Length RadosPartHandle::size() {
    return len_;
}

Length RadosPartHandle::estimate() {
    return size();
}

Offset RadosPartHandle::position() {
    return {pos_};
}

Offset RadosPartHandle::seek(const Offset& offset) {
    pos_ = eckit::Length(offset);
    return offset;
}

bool RadosPartHandle::canSeek() const {
    return true;
}

std::string RadosPartHandle::title() const {
    return object_.name();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
