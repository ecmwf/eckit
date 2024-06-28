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

namespace eckit {

RadosAsyncHandle::RadosAsyncHandle(const RadosObject& object, size_t maxAioBuffSize) :
    RadosHandle(object), maxAioBuffSize_(maxAioBuffSize) {}

void RadosAsyncHandle::print(std::ostream& s) const {
    s << "RadosAsyncHandle[" << object_.str() << ";";
}

long RadosAsyncHandle::write(const void* buffer, long length) {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

    ASSERT(length);
    ASSERT(opened_);
    ASSERT(write_);

    ASSERT(length <= RadosCluster::instance().maxWriteSize());
    ASSERT((offset_ + length) <= RadosCluster::instance().maxObjectSize());

    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.emplace_back(new eckit::RadosAIO());

    if (first_write_) {
    
        eckit::StatsTimer st{"rados_aio_write_full", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};

        RADOS_CALL(
            rados_aio_write_full(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                comps_.back()->comp_,
                reinterpret_cast<const char*>(buffer), 
                length
            )
        );

        first_write_ = false;

    } else {

        eckit::StatsTimer st{"rados_aio_write", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};

        RADOS_CALL(
            rados_aio_write(
                RadosCluster::instance().ioCtx(object_), 
                object_.name().c_str(),
                comps_.back()->comp_,
                reinterpret_cast<const char*>(buffer), 
                length, offset_
            )
        );

    }

    offset_ += length;

    return length;

}

void RadosAsyncHandle::flush() {

    using namespace std::placeholders;
    eckit::Timer& timer = eckit::RadosCluster::instance().radosCallTimer();
    eckit::RadosIOStats& stats = eckit::RadosCluster::instance().stats();

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
        eckit::StatsTimer st{"rados_obj_aio_wait_for_complete", timer, std::bind(&eckit::RadosIOStats::logMdOperation, &stats, _1, _2)};
        RADOS_CALL(rados_aio_wait_for_complete(comp->comp_));
    }
    comps_.clear();

}

void RadosAsyncHandle::close() {

    RadosHandle::close();

    comps_.clear();

}

}  // namespace eckit