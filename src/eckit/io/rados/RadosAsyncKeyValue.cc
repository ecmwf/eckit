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

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosKeyValue.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosAsyncKeyValue::RadosAsyncKeyValue(const eckit::URI& uri, size_t maxAioBuffSize) :
    RadosKeyValue(uri), maxAioBuffSize_(maxAioBuffSize) {}

RadosAsyncKeyValue::RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid,
                                       size_t maxAioBuffSize) :
    RadosKeyValue(pool, nspace, oid), maxAioBuffSize_(maxAioBuffSize) {}

RadosAsyncKeyValue::~RadosAsyncKeyValue() {
    /// @note: modifications which were not flushed would otherwise fail unnoticed
    try {
        flush();
    }
    catch (std::exception& e) {
        eckit::Log::error() << "RadosAsyncKeyValue " << str() << " failed to flush: " << e.what() << std::endl;
    }
}

void RadosAsyncKeyValue::track(std::unique_ptr<eckit::RadosAIO> comp) const {
    comps_.push_back(std::move(comp));
}

void RadosAsyncKeyValue::ensureCreated() const {
    if (comps_.size() >= maxAioBuffSize_) {
        flush();
    }
    track(RadosKeyValue::ensureCreatedAsync());
}

long RadosAsyncKeyValue::put(const std::string& key, const void* buf, const long& len) const {
    if (comps_.size() >= maxAioBuffSize_) {
        flush();
    }
    long res = 0;
    track(RadosKeyValue::putAsync(key, buf, len, res));
    return res;
}

void RadosAsyncKeyValue::remove(const std::string& key) const {
    if (comps_.size() >= maxAioBuffSize_) {
        flush();
    }
    track(RadosKeyValue::removeAsync(key));
}

void RadosAsyncKeyValue::flush() const {
    RadosAIO::waitForAll(comps_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
