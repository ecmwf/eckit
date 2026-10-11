// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <map>
#include <memory>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/cache/MemoryUsage.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo::cache {


/**
 * @brief Values shared in the process, by key, within a capacity (as mir's InMemoryCache)
 * @details Values report their footprint (T::footprint(), a MemoryUsage). When the cached values exceed the capacity,
 * the least recently used are evicted, except those in use (which remain valid for their users).
 */
template <typename T>
class InMemoryCache : private MemoryCache {
public:

    // -- Types

    using key_type   = std::string;
    using value_type = std::shared_ptr<const T>;

    struct Statistics {
        size_t hits      = 0;
        size_t misses    = 0;
        size_t evictions = 0;
    };

    // -- Constructors

    explicit InMemoryCache(const MemoryUsage& capacity) : capacity_(capacity) {}

    // -- Destructor

    ~InMemoryCache() override = default;

    // -- Methods

    /// Cached, or made (make() returns a value_type)
    template <typename Make>
    value_type get(const key_type& key, const Make& make) {
        {
            util::lock_guard<util::recursive_mutex> lock(mutex_);

            if (auto it = entries_.find(key); it != entries_.end()) {
                ++statistics_.hits;
                it->second.last_use = ++clock_;
                return it->second.value;
            }

            ++statistics_.misses;
        }

        // made without holding the cache, as it can take long
        value_type value = make();
        ASSERT(value);

        util::lock_guard<util::recursive_mutex> lock(mutex_);

        auto [it, inserted] = entries_.try_emplace(key, Entry{value, value->footprint(), ++clock_});
        if (inserted) {
            evict();
        }

        return it->second.value;
    }

    MemoryUsage usage() const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        MemoryUsage sum;
        for (const auto& [key, entry] : entries_) {
            sum += entry.footprint;
        }
        return sum;
    }

    MemoryUsage capacity() const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return capacity_;
    }

    void capacity(const MemoryUsage& capacity) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        capacity_ = capacity;
        evict();
    }

    Statistics statistics() const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return statistics_;
    }

    size_t size() const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return entries_.size();
    }

    void clear() {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        statistics_.evictions += entries_.size();
        entries_.clear();
    }

private:

    // -- Types

    struct Entry {
        value_type value;
        MemoryUsage footprint;
        size_t last_use;
    };

    // -- Members

    mutable util::recursive_mutex mutex_;
    std::map<key_type, Entry> entries_;
    MemoryUsage capacity_;
    Statistics statistics_;
    size_t clock_ = 0;

    // -- Methods

    void evict() {
        while (usage().exceeds(capacity_)) {
            // least recently used, not in use beyond the cache
            auto victim = entries_.end();
            for (auto it = entries_.begin(); it != entries_.end(); ++it) {
                if (it->second.value.use_count() == 1 &&
                    (victim == entries_.end() || it->second.last_use < victim->second.last_use)) {
                    victim = it;
                }
            }

            if (victim == entries_.end()) {
                return;  // over capacity, all in use
            }

            entries_.erase(victim);
            ++statistics_.evictions;
        }
    }

    // -- Overridden methods

    bytes_size_t footprint() const override { return usage().total(); }
    void purge() override { clear(); }
};


}  // namespace eckit::geo::cache
