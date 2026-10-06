// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/MemoryCache.h"

#include <algorithm>
#include <vector>


namespace eckit::geo::cache {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


static std::vector<MemoryCache*> CACHES;


MemoryCache::bytes_size_t MemoryCache::total_footprint() {
    lock_type lock;
    return std::accumulate(CACHES.begin(), CACHES.end(), static_cast<bytes_size_t>(0),
                           [](bytes_size_t sum, const auto* cache) { return sum + cache->footprint(); });
}


void MemoryCache::total_purge() {
    lock_type lock;
    std::for_each(CACHES.begin(), CACHES.end(), [](auto* cache) { cache->purge(); });
}


MemoryCache::MemoryCache() {
    lock_type lock;
    CACHES.emplace_back(this);
}


}  // namespace eckit::geo::cache
