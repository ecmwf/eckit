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
