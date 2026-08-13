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

size_t RadosAsyncHandle::totalCompsBuffSize() const {
    size_t buff_size = 0;
    for (const auto& comp : comps_) {
        buff_size += comp->len_;
    }
    return buff_size;
}

long RadosAsyncHandle::write(const void* buffer, long length) {

    ASSERT(length);
    ASSERT(opened_);
    ASSERT(write_);

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    ASSERT(totalCompsBuffSize() < maxAioBuffSize_);

    auto& handle = comps_.emplace_back(std::make_unique<eckit::RadosAIO>());
    handle->len_ = length;

    if (first_write_) {
        RADOS_CALL(rados_aio_write_full(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                        comps_.back()->comp_, reinterpret_cast<const char*>(buffer), length));
        first_write_ = false;
    }
    else {
        RADOS_CALL(rados_aio_write(RadosCluster::instance().ioCtx(object_), object_.name().c_str(),
                                   comps_.back()->comp_, reinterpret_cast<const char*>(buffer), length, offset_));
    }

    offset_ += length;

    return length;
}

void RadosAsyncHandle::flush() {
    /// @note: not correct! aio_flush waits for safe on all AIOs for an IoCtx for an entire pool/namespace
    ///   where AIOs from multiple RadosAsyncHandles (belonging to a same process) for
    ///   objects on the same pool could be ongoing.
    ///   Alternative: wait_for_complete on each AIO in comps_.
    ///   Alternative: have an IoCtx for each application in RadosCluster
    ///   Alternative: call rados_aio_flush in RadosStore::flush for all IoCtx for all open handles.
    ///     Since they all ??must belong to a same pool and namespace?? a single aio_flush should suffice?
    // RADOS_CALL(rados_aio_flush(RadosCluster::instance().ioCtx(object_)));
    // comps_.clear();

    for (const auto& comp : comps_) {
        comp->waitForComplete();
    }
    comps_.clear();
}

void RadosAsyncHandle::close() {
    RadosHandle::close();
    comps_.clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
