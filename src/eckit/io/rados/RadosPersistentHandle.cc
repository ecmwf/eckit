/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosPersistentHandle.h"
// #include "eckit/exception/Exceptions.h"

namespace eckit {

RadosPersistentHandle::RadosPersistentHandle(const RadosObject& object, bool persist_on_write, size_t maxAioBuffSize) :
    RadosHandle(object), persist_on_write_(persist_on_write), maxAioBuffSize_(maxAioBuffSize) {}

void RadosPersistentHandle::print(std::ostream& s) const {
    s << "RadosPersistentHandle[" << object_.str() << ";";
}

long RadosPersistentHandle::write(const void* buffer, long length) {

    ASSERT(length);
    ASSERT(opened_);
    ASSERT(write_);

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.emplace_back(new eckit::RadosAIO());

    if (first_write_) {

        RADOS_CALL(
            rados_aio_write_full(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                comps_.back()->comp_,
                reinterpret_cast<const char*>(buffer), 
                length
            )
        );

        if (persist_on_write_) {
            RADOS_CALL(rados_aio_wait_for_safe(comps_.back()->comp_));
            comps_.pop_back();
        } else {
            RADOS_CALL(rados_aio_wait_for_complete(comps_.back()->comp_));
        } 

        first_write_ = false;

    } else {

        RADOS_CALL(
            rados_aio_write(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                comps_.back()->comp_,
                reinterpret_cast<const char*>(buffer), 
                length, offset_
            )
        );
        
        if (persist_on_write_) {
            RADOS_CALL(rados_aio_wait_for_safe(comps_.back()->comp_));
            comps_.pop_back();
        } else {
            RADOS_CALL(rados_aio_wait_for_complete(comps_.back()->comp_));
        }

    }

    offset_ += length;

    return length;

}

void RadosPersistentHandle::flush() {

    if (persist_on_write_) return;

    /// @note: not correct! aio_flush waits for safe on all AIOs for an IoCtx for an entire pool/namespace
    ///   where AIOs from multiple RadosPersistentHandles (belonging to a same process) for 
    ///   objects on the same pool could be ongoing.
    ///   Alternative: wait_for_safe on each AIO in comps_.
    ///   Alternative: have an IoCtx for each application in RadosCluster
    ///   Alternative: call rados_aio_flush in RadosStore::flush for all IoCtx for all open handles.
    ///     Since they all ??must belong to a same pool and namespace?? a single aio_flush should suffice?
    // RADOS_CALL(rados_aio_flush(RadosCluster::instance().ioCtx(object_)));
    // comps_.clear();

    for (const auto& comp : comps_)
        RADOS_CALL(rados_aio_wait_for_safe(comp->comp_));
    comps_.clear();

}

void RadosPersistentHandle::close() {

    RadosHandle::close();

    if (persist_on_write_) ASSERT(comps_.size() == 0);

    comps_.clear();

}

}  // namespace eckit