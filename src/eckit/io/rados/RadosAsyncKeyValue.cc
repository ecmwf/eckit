// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosAsyncKeyValue.h"

#include <rados/librados.h>

#include <cstddef>
#include <exception>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosKeyValue.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosAsyncKeyValue::RadosAsyncKeyValue(const eckit::URI& uri, size_t maxPendingOps) :
    RadosKeyValue(uri), maxPendingOps_(maxPendingOps) {}

RadosAsyncKeyValue::RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& name,
                                       size_t maxPendingOps) :
    RadosKeyValue(pool, nspace, name), maxPendingOps_(maxPendingOps) {}

RadosAsyncKeyValue::~RadosAsyncKeyValue() {
    /// @note: modifications which were not flushed would otherwise fail unnoticed
    try {
        flush();
    }
    catch (std::exception& e) {
        eckit::Log::error() << "RadosAsyncKeyValue " << str() << " failed to flush: " << e.what() << std::endl;
    }
}

void RadosAsyncKeyValue::submit(RadosWriteOp& op) {

    if (pending_.size() >= maxPendingOps_) {
        flush();
    }

    /// @note: only kept once the operation has been submitted. Waiting on a completion which was
    ///   never submitted would block forever.
    auto comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(rados_aio_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object()), comp->comp_, name().c_str(),
                                          nullptr, 0));

    pending_.push_back(std::move(comp));
}

void RadosAsyncKeyValue::flush() {
    RadosAIO::waitForAll(pending_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
