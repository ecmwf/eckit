// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/MemoryCache.h"

#include <algorithm>

#include "eckit/cache/LibEcKitCache.h"


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


MemoryUsage MemoryCaches::capacity() const {
    util::lock_guard<util::recursive_mutex> lock(mutex_);
    return capacity_.value_or(LibEcKitCache::totalMemoryCapacity());
}


void MemoryCaches::capacity(const MemoryUsage& capacity) {
    util::lock_guard<util::recursive_mutex> lock(mutex_);
    capacity_ = capacity;
    fit();
}


void MemoryCaches::fit() {
    util::lock_guard<util::recursive_mutex> lock(mutex_);

    MemoryUsage usage;
    for (const auto* cache : caches_) {
        usage += cache->footprint() + cache->reserved();
    }

    auto over = usage - capacity();
    for (auto* cache : caches_) {
        if (over == MemoryUsage{}) {
            break;
        }
        over -= cache->release(over);
    }
}


void MemoryCacheBase::registerCache() {
    util::lock_guard<util::recursive_mutex> lock(group_.mutex_);
    group_.caches_.push_back(this);
}


void MemoryCacheBase::unregisterCache() {
    util::lock_guard<util::recursive_mutex> lock(group_.mutex_);

    auto& v = group_.caches_;
    v.erase(std::remove(v.begin(), v.end(), this), v.end());
}


}  // namespace eckit::cache
