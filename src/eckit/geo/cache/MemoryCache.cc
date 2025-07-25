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


#include "eckit/geo/cache/MemoryCache.h"

#include <algorithm>
#include <vector>


namespace eckit::geo::cache {


static util::recursive_mutex MUTEX;
static std::vector<MemoryCache*> CACHES;


MemoryCache::bytes_size_t MemoryCache::total_footprint() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    return std::accumulate(CACHES.begin(), CACHES.end(), static_cast<bytes_size_t>(0),
                           [](bytes_size_t sum, const auto* cache) { return sum + cache->footprint(); });
}


void MemoryCache::total_purge() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    std::for_each(CACHES.begin(), CACHES.end(), [](auto* cache) { cache->purge(); });
}


MemoryCache::MemoryCache() {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    CACHES.emplace_back(this);
}


}  // namespace eckit::geo::cache
