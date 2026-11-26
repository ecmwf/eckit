/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosAsyncHandle.h"

#include <rados/librados.h>

#include <cstddef>
#include <memory>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosAsyncHandle::RadosAsyncHandle(const RadosObject& object, size_t maxAioBuffSize) :
    RadosHandle(object), maxAioBuffSize_(maxAioBuffSize) {}

void RadosAsyncHandle::print(std::ostream& out) const {
    out << "RadosAsyncHandle[" << object_.str() << "]";
}

long RadosAsyncHandle::write(const void* buffer, long length) {

    ASSERT(opened_);
    ASSERT(write_);

    if (length == 0) {
        return 0;
    }

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    /// @note: bound the amount of data held by in-flight writes
    if (pendingBytes_ >= maxAioBuffSize_) {
        flush();
    }

    /// @note: only track the completion once the operation has been submitted. Waiting on a
    ///   completion which was never submitted would block forever.
    auto comp  = std::make_unique<eckit::RadosAIO>();
    comp->len_ = length;

    if (truncate_) {
        RadosWriteOp op;
        firstWrite(op, buffer, length);
        RADOS_CALL(rados_aio_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), comp->comp_,
                                              object_.name().c_str(), NULL, 0));
        truncate_ = false;
    }
    else {
        RADOS_CALL(rados_aio_write(RadosCluster::instance().ioCtx(object_), object_.name().c_str(), comp->comp_,
                                   reinterpret_cast<const char*>(buffer), length, offset_));
    }

    comps_.push_back(std::move(comp));
    pendingBytes_ += length;

    offset_ += length;

    return length;
}

void RadosAsyncHandle::flush() {
    pendingBytes_ = 0;
    RadosAIO::waitForAll(comps_);
}

void RadosAsyncHandle::close() {
    ASSERT(opened_);
    try {
        flush();
    }
    catch (...) {
        RadosHandle::close();
        throw;
    }
    RadosHandle::close();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
