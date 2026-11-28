// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <utility>

#include "eckit/cache/MemoryCache.h"
#include "eckit/filesystem/PathName.h"


namespace eckit::spec {
class Spec;
}


namespace eckit::geo::cache {


class RecordCache {
public:

    virtual ~RecordCache() = default;

    virtual eckit::cache::MemoryUsage footprint() const = 0;
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

    RecordCacheT(eckit::cache::MemoryCaches& group, const Loader& loader) : cache_(group), loader_(loader) {}

    bool has(const key_type& key) const { return cache_.find(key) != nullptr; }

    const value_type& get(const key_type& key) {
        if (const auto* cached = cache_.find(key)) {
            return *cached;
        }

        value_type record;
        loader_.load(key, record);
        return cache_.insert(key, std::move(record));
    }

    const value_type& set(const key_type& key, value_type&& record) { return cache_.insert(key, std::move(record)); }

    void purge() { cache_.purge(); }

private:

    eckit::cache::MemoryCacheT<key_type, value_type> cache_;
    const Loader& loader_;
};


}  // namespace eckit::geo::cache
