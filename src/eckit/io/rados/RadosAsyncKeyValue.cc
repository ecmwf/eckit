/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosAsyncKeyValue.h"

namespace eckit {

RadosAsyncKeyValue::RadosAsyncKeyValue(const eckit::URI& uri, size_t maxAioBuffSize) :
    RadosKeyValue(uri), maxAioBuffSize_(maxAioBuffSize) {}

RadosAsyncKeyValue::RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid, size_t maxAioBuffSize) :
    RadosKeyValue(pool, nspace, oid), maxAioBuffSize_(maxAioBuffSize) {}

void RadosAsyncKeyValue::ensureCreated() {

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::ensureCreatedAsync();
    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.emplace_back(comp.release());

}

long RadosAsyncKeyValue::put(const std::string& key, const void* buf, const long& len) {

    long res;

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::putAsync(key, buf, len, res);
    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.emplace_back(comp.release());

    return res;

}

void RadosAsyncKeyValue::remove(const std::string& key) {

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::removeAsync(key);
    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.emplace_back(comp.release());

    return;

}

void RadosAsyncKeyValue::flush() {

    for (const auto& comp : comps_)
        RADOS_CALL(rados_aio_wait_for_complete(comp->comp_));
    comps_.clear();

}

}  // namespace eckit