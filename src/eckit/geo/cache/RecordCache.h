// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::spec {
class Spec;
}


namespace eckit::geo::cache {


class RecordCache {
public:

    using bytes_size_t = MemoryCache::bytes_size_t;

    virtual ~RecordCache() = default;

    virtual MemoryCache::bytes_size_t footprint() const = 0;
    virtual void read(const PathName&)                  = 0;
};


template <typename Value, typename Key>
class RecordCacheT {
public:

    using key_type   = Key;
    using value_type = Value;

    struct Loader {
        virtual ~Loader()                                     = default;
        virtual void load(const key_type&, value_type&) const = 0;
    };

    explicit RecordCacheT(const Loader& loader) : loader_(loader) {}

    bool has(const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return cache_.contains(key);
    }

    const value_type& get(const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        if (cache_.contains(key)) {
            return cache_[key];
        }

        auto& record = cache_[key];
        loader_.load(key, record);

        return record;
    }

    const value_type& set(const key_type& key, value_type&& record) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return cache_[key] = std::move(record);
    }

    void purge() {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        cache_.purge();
    }

private:

    util::recursive_mutex mutex_;
    MemoryCacheT<key_type, value_type> cache_;
    const Loader& loader_;
};


}  // namespace eckit::geo::cache
