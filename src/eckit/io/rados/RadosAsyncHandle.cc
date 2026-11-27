// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosAsyncHandle.h"

#include <rados/librados.h>

#include <cstddef>
#include <memory>
#include <ostream>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosAsyncHandle::RadosAsyncHandle(const RadosObject& object, size_t maxPendingBytes) :
    RadosHandle(object), maxPendingBytes_(maxPendingBytes) {}

RadosAsyncHandle::~RadosAsyncHandle() = default;

void RadosAsyncHandle::print(std::ostream& out) const {
    out << "RadosAsyncHandle[" << object_.str() << "]";
}

void RadosAsyncHandle::submit(RadosWriteOp& op, long length) {

    /// @note: bound the amount of data held by writes in flight
    if (pendingBytes_ >= maxPendingBytes_) {
        flush();
    }

    /// @note: only kept once the operation has been submitted. Waiting on a completion which was
    ///   never submitted would block forever.
    auto comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(rados_aio_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), comp->comp_,
                                          object_.name().c_str(), nullptr, 0));

    pending_.push_back(std::move(comp));
    pendingBytes_ += length;
}

void RadosAsyncHandle::flush() {
    pendingBytes_ = 0;
    RadosAIO::waitForAll(pending_);
}

void RadosAsyncHandle::close() {

    ASSERT(isOpen());

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
