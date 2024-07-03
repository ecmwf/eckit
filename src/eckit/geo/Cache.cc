/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Cache.h"

#include <algorithm>
#include <vector>


namespace eckit::geo {


static util::recursive_mutex MUTEX;
static std::vector<Cache*> CACHES;


Cache::bytes_size_t Cache::total_footprint() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    return std::accumulate(CACHES.begin(), CACHES.end(), static_cast<bytes_size_t>(0),
                           [](bytes_size_t sum, const auto* cache) { return sum + cache->footprint(); });
}


void Cache::total_purge() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    std::for_each(CACHES.begin(), CACHES.end(), [](auto* cache) { cache->purge(); });
}


Cache::Cache() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    CACHES.emplace_back(this);
}


}  // namespace eckit::geo
