/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosPersistentKeyValue.h"

namespace eckit {

RadosPersistentKeyValue::RadosPersistentKeyValue(
    const eckit::URI& uri, bool persist_on_write, size_t maxAioBuffSize) :
    RadosKeyValue(uri), persist_on_write_(persist_on_write), maxAioBuffSize_(maxAioBuffSize) {}

RadosPersistentKeyValue::RadosPersistentKeyValue(
    const std::string& pool, const std::string& nspace, const std::string& oid,
    bool persist_on_write, size_t maxAioBuffSize) :
    RadosKeyValue(pool, nspace, oid), persist_on_write_(persist_on_write), maxAioBuffSize_(maxAioBuffSize) {}

void RadosPersistentKeyValue::ensureCreated() {

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::ensureCreated(persist_on_write_);

    if (persist_on_write_) return;

    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.push_back(std::move(comp));

}

long RadosPersistentKeyValue::put(const std::string& key, const void* buf, const long& len) {

    long res;

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::put(key, buf, len, res, persist_on_write_);

    if (persist_on_write_) return res;

    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.push_back(std::move(comp));

    return res;

}

void RadosPersistentKeyValue::remove(const std::string& key) {

    std::unique_ptr<eckit::RadosAIO> comp = RadosKeyValue::remove(key, persist_on_write_);

    if (persist_on_write_) return;

    ASSERT(comps_.size() < maxAioBuffSize_);
    comps_.push_back(std::move(comp));

    return;

}

void RadosPersistentKeyValue::flush() {

    if (persist_on_write_) return;

    for (const auto& comp : comps_)
        RADOS_CALL(rados_aio_wait_for_safe(comp->comp_));
    comps_.clear();

}

}  // namespace eckit