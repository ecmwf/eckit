// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosHandle.h"

#include <rados/librados.h>

#include <algorithm>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosCall.h"
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

    /// @note: fails here, rather than on the first write, if the pool does not exist
    RadosCluster::instance().ioCtx(object_);

    open();

    write_    = true;
    truncate_ = true;
}

void RadosHandle::firstWrite(RadosWriteOp& op, const void* buffer, long length) const {

    const char* buf = reinterpret_cast<const char*>(buffer);

    if (offset_ == 0) {
        /// @note: replaces the whole content, creating the object if needed
        rados_write_op_write_full(op.op_, buf, length);
        return;
    }

    rados_write_op_create(op.op_, LIBRADOS_CREATE_IDEMPOTENT, NULL);
    rados_write_op_truncate(op.op_, 0);
    rados_write_op_write(op.op_, buf, length, offset_);
}

void RadosHandle::checkWrite(long length) const {

    const long long maxWrite  = RadosCluster::instance().maxWriteSize();
    const long long maxObject = RadosCluster::instance().maxObjectSize();

    if (length > maxWrite) {
        std::ostringstream oss;
        oss << "Write of " << length << " bytes to " << object_.str() << " exceeds the maximum write size of "
            << maxWrite << " bytes";
        throw BadValue(oss.str(), Here());
    }

    if (static_cast<long long>(offset_) + length > maxObject) {
        std::ostringstream oss;
        oss << "Write of " << length << " bytes at offset " << offset_ << " of " << object_.str()
            << " exceeds the maximum object size of " << maxObject << " bytes";
        throw BadValue(oss.str(), Here());
    }
}

long RadosHandle::read(void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(!write_);

    /// @note: librados rejects reads larger than INT_MAX. A short read is fine for the caller.
    length = std::min(length, radosMaxReadSize);

    int len = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                    reinterpret_cast<char*>(buffer), length, offset_));

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(write_);

    if (length == 0) {
        return 0;
    }

    checkWrite(length);

    if (truncate_) {
        RadosWriteOp op;
        firstWrite(op, buffer, length);
        RADOS_CALL(
            rados_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), object_.name().c_str(), NULL, 0));
        truncate_ = false;
    }
    else {
        RADOS_CALL(rados_write(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                               reinterpret_cast<const char*>(buffer), length, offset_));
    }

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

    /// @note: nothing was written, so the object is left existing and empty
    if (write_ && truncate_) {
        truncate_ = false;
        RadosCluster::instance().truncate(object_);
    }
}

Offset RadosHandle::position() {
    return offset_;
}

std::string RadosHandle::title() const {
    return PathName::shorten(object_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
