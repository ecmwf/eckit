// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/MemoryCache.h"

#include <algorithm>


namespace eckit::cache {


MemoryUsage MemoryCaches::footprint() const {
    util::lock_guard<util::recursive_mutex> lock(mutex_);

    MemoryUsage sum;
    for (const auto* cache : caches_) {
        sum += cache->footprint();
    }
    return sum;
}


void MemoryCaches::purge() {
    util::lock_guard<util::recursive_mutex> lock(mutex_);

    for (auto* cache : caches_) {
        cache->purge();
    }
}


void MemoryCacheBase::register_cache() {
    util::lock_guard<util::recursive_mutex> lock(group_.mutex_);
    group_.caches_.push_back(this);
}


void MemoryCacheBase::unregister_cache() {
    util::lock_guard<util::recursive_mutex> lock(group_.mutex_);

    auto& v = group_.caches_;
    v.erase(std::remove(v.begin(), v.end(), this), v.end());
}


}  // namespace eckit::cache
