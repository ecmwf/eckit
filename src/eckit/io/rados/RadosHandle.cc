// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosHandle.h"

#include <rados/librados.h>

#include <algorithm>
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
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosHandle::RadosHandle(const RadosObject& object) : object_(object) {}

RadosHandle::~RadosHandle() {
    if (isOpen()) {
        eckit::Log::error() << *this << " not closed before destruction." << std::endl;
    }
}

void RadosHandle::print(std::ostream& s) const {
    s << "RadosHandle[" << object_.str() << ']';
}

std::string RadosHandle::title() const {
    return PathName::shorten(object_.str());
}

Length RadosHandle::size() {
    return RadosCluster::instance().size(object_);
}

Length RadosHandle::openForRead() {

    ASSERT(!isOpen());

    const Length length = size();

    offset_ = 0;
    mode_   = Mode::Read;

    return length;
}

void RadosHandle::openForWrite(const Length&) {

    ASSERT(!isOpen());

    /// @note: fails here, rather than on the first write, if the pool does not exist
    RadosCluster::instance().ioCtx(object_);

    offset_   = 0;
    truncate_ = true;
    mode_     = Mode::Write;
}

long RadosHandle::read(void* buffer, long length) {

    ASSERT(mode_ == Mode::Read);

    /// @note: a short read is fine for the caller
    length = std::min(length, radosMaxReadSize);

    const int len = RADOS_CALL(rados_read(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                          static_cast<char*>(buffer), length, offset_));

    offset_ += len;

    return len;
}

long RadosHandle::write(const void* buffer, long length) {

    ASSERT(mode_ == Mode::Write);

    if (length == 0) {
        return 0;
    }

    const long long maxWrite  = RadosCluster::instance().maxWriteSize();
    const long long maxObject = RadosCluster::instance().maxObjectSize();

    if (length > maxWrite || static_cast<long long>(offset_) + length > maxObject) {
        std::ostringstream oss;
        oss << "Write of " << length << " bytes at offset " << offset_ << " of " << object_.str()
            << " exceeds the maximum write size of " << maxWrite << " bytes or the maximum object size of " << maxObject
            << " bytes";
        throw BadValue(oss.str(), Here());
    }

    const char* buf = static_cast<const char*>(buffer);

    RadosWriteOp op;

    if (!truncate_) {
        rados_write_op_write(op.op_, buf, length, offset_);
    }
    else if (offset_ == 0) {
        /// @note: replaces the whole content, creating the object if needed
        rados_write_op_write_full(op.op_, buf, length);
    }
    else {
        rados_write_op_create(op.op_, LIBRADOS_CREATE_IDEMPOTENT, nullptr);
        rados_write_op_truncate(op.op_, 0);
        rados_write_op_write(op.op_, buf, length, offset_);
    }

    submit(op, length);

    truncate_ = false;
    offset_ += length;

    return length;
}

void RadosHandle::submit(RadosWriteOp& op, long) {
    RADOS_CALL(
        rados_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), object_.name().c_str(), nullptr, 0));
}

void RadosHandle::flush() {}

Offset RadosHandle::seek(const Offset& offset) {
    ASSERT(isOpen());
    ASSERT(offset >= Offset(0));
    offset_ = offset;
    return offset;
}

void RadosHandle::rewind() {
    ASSERT(isOpen());
    offset_ = 0;
}

Offset RadosHandle::position() {
    return offset_;
}

void RadosHandle::close() {

    ASSERT(isOpen());

    const bool empty = mode_ == Mode::Write && truncate_;

    mode_     = Mode::Closed;
    truncate_ = false;

    /// @note: nothing was written, so the object is left existing and empty
    if (empty) {
        RadosCluster::instance().truncate(object_);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
