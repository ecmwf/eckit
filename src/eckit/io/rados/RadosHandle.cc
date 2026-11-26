// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosHandle.h"

#include <rados/librados.h>

#include <algorithm>
#include <limits>
#include <ostream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[" << object_.str() << ']';
}

RadosHandle::RadosHandle(const RadosObject& object) : object_(object), offset_(0), opened_(false), write_(false) {}

RadosHandle::~RadosHandle() {
    if (opened_) {
        eckit::Log::error() << "RadosHandle not closed before destruction." << std::endl;
    }
}

void RadosHandle::open() {

    ASSERT(!opened_);

    offset_ = 0;

    opened_ = true;
}

Length RadosHandle::size() {
    return RadosCluster::instance().size(object_);
}

Length RadosHandle::openForRead() {

    ASSERT(!opened_);

    Length length = RadosCluster::instance().size(object_);

    open();

    write_ = false;

    return length;
}

void RadosHandle::openForWrite(const Length& length) {

    ASSERT(!opened_);

    /// @note: creates the object if it does not exist, and empties it otherwise
    RadosCluster::instance().truncate(object_);

    open();

    write_ = true;
}

long RadosHandle::read(void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(!write_);

    /// @note: librados rejects reads larger than INT_MAX. A short read is fine for the caller.
    length = std::min<long>(length, std::numeric_limits<int>::max());

    int len = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                    reinterpret_cast<char*>(buffer), length, offset_));

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {

    ASSERT(length);
    ASSERT(opened_);
    ASSERT(write_);

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    RADOS_CALL(rados_write(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                           reinterpret_cast<const char*>(buffer), length, offset_));

    offset_ += length;

    return length;
}

void RadosHandle::flush() {}

eckit::Offset RadosHandle::seek(const eckit::Offset& offset) {
    offset_ = offset;
    return offset_;
}

void RadosHandle::rewind() {
    offset_ = 0;
}

void RadosHandle::close() {
    ASSERT(opened_);
    opened_ = false;
}

Offset RadosHandle::position() {
    return offset_;
}

std::string RadosHandle::title() const {
    return PathName::shorten(object_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
