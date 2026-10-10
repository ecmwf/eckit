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
 * @details Values report their footprint (T::footprint(), a MemoryUsage). On a miss, room is made for the value before
 * making it (as estimated), evicting the least recently used values not in use (which remain valid for their users).
 * Values are made once, even if requested concurrently, and are cached even if beyond the capacity.
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

    /// Cached, or made (make() returns a value_type) after making room for its estimated footprint
    template <typename Make>
    value_type get(const key_type& key, const MemoryUsage& estimate, const Make& make) {
        // requests of the same key are serialised, so the value is made once (the others find it cached)
        util::lock_guard<util::recursive_mutex> making(key_mutex(key));

        {
            util::lock_guard<util::recursive_mutex> lock(mutex_);

            if (auto it = entries_.find(key); it != entries_.end()) {
                ++statistics_.hits;
                it->second.last_use = ++clock_;
                return it->second.value;
            }

            ++statistics_.misses;
            reserved_[key] = estimate;
            evict();
        }

        // made without holding the cache, as it can take long
        value_type value;
        try {
            value = make();
            ASSERT(value);
        }
        catch (...) {
            util::lock_guard<util::recursive_mutex> lock(mutex_);
            reserved_.erase(key);
            throw;
        }

        util::lock_guard<util::recursive_mutex> lock(mutex_);

        reserved_.erase(key);
        entries_.try_emplace(key, Entry{value, value->footprint(), ++clock_});
        evict();  // the footprint can differ from the estimate

        return value;
    }

    /// Cached, or made (make() returns a value_type) then making room for it
    template <typename Make>
    value_type get(const key_type& key, const Make& make) {
        return get(key, MemoryUsage{}, make);
    }

    /// Footprint of the cached values
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
    std::map<key_type, MemoryUsage> reserved_;  // estimates, of values being made
    std::map<key_type, std::unique_ptr<util::recursive_mutex>> key_mutexes_;
    MemoryUsage capacity_;
    Statistics statistics_;
    size_t clock_ = 0;

    // -- Methods

    /// One per key, never released (the number of keys is bounded)
    util::recursive_mutex& key_mutex(const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        auto& m = key_mutexes_[key];
        if (!m) {
            m = std::make_unique<util::recursive_mutex>();
        }
        return *m;
    }

    /// Evict the least recently used values not in use, until the values and those being made (estimated) fit
    void evict() {
        auto usage = this->usage();
        for (const auto& [key, estimate] : reserved_) {
            usage += estimate;
        }

        while (usage.exceeds(capacity_)) {
            const auto over = usage - capacity_;

            auto victim = entries_.end();
            for (auto it = entries_.begin(); it != entries_.end(); ++it) {
                const auto& entry = it->second;

                const bool in_use   = entry.value.use_count() > 1;
                const bool relieves = (over.memory() > 0 && entry.footprint.memory() > 0) ||
                                      (over.shared() > 0 && entry.footprint.shared() > 0);

                if (!in_use && relieves && (victim == entries_.end() || entry.last_use < victim->second.last_use)) {
                    victim = it;
                }
            }

            if (victim == entries_.end()) {
                return;  // beyond capacity, nothing else to evict
            }

            usage -= victim->second.footprint;
            entries_.erase(victim);
            ++statistics_.evictions;
        }
    }

    // -- Overridden methods

    bytes_size_t footprint() const override { return usage().total(); }
    void purge() override { clear(); }
};


}  // namespace eckit::geo::cache
