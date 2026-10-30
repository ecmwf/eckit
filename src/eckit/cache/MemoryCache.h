// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "eckit/cache/MemoryUsage.h"
#include "eckit/cache/Policy.h"
#include "eckit/cache/mutex.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::cache {


class MemoryCacheBase;


/// In-process caches of a library (e.g. LibEcKitGeo::memoryCaches), reported and released as a whole
class MemoryCaches {
public:

    // -- Constructors

    MemoryCaches() = default;

    MemoryCaches(const MemoryCaches&) = delete;
    MemoryCaches(MemoryCaches&&)      = delete;

    // -- Destructor

    ~MemoryCaches() = default;

    // -- Operators

    MemoryCaches& operator=(const MemoryCaches&) = delete;
    MemoryCaches& operator=(MemoryCaches&&)      = delete;

    // -- Methods

    MemoryUsage footprint() const;

    /// Release the cached values (see MemoryCache::purge and MemoryCacheT::purge)
    void purge();

private:

    // -- Members

    mutable util::recursive_mutex mutex_;
    std::vector<MemoryCacheBase*> caches_;

    // -- Friends

    friend class MemoryCacheBase;
};


/// In-process cache, of a group of caches (outliving it)
class MemoryCacheBase {
public:

    // -- Constructors

    MemoryCacheBase(const MemoryCacheBase&) = delete;
    MemoryCacheBase(MemoryCacheBase&&)      = delete;

    // -- Destructor

    virtual ~MemoryCacheBase() = default;

    // -- Operators

    MemoryCacheBase& operator=(const MemoryCacheBase&) = delete;
    MemoryCacheBase& operator=(MemoryCacheBase&&)      = delete;

    // -- Methods

    virtual MemoryUsage footprint() const = 0;

    /// Release the cached values
    virtual void purge() = 0;

protected:

    // -- Constructors

    explicit MemoryCacheBase(MemoryCaches& group) : group_(group) {}

    // -- Methods

    /**
     * Call last in derived constructors and first in derived destructors, so a concurrent purge or footprint (of the
     * group) never sees unconstructed or destructed members
     */
    void register_cache();
    void unregister_cache();

private:

    // -- Members

    MemoryCaches& group_;
};


/**
 * @brief Values shared in the process by key, bounded by a memory capacity (private and shared, accounted separately)
 * @details Room is made before making a value, using an estimate, as making can peak above the capacity. Values not in
 * use are evicted, in the order of the eviction policy (see Policy, default least recently used), so eviction never
 * invalidates a value. A value is made once even if requested concurrently, and cached even if larger than the
 * capacity (refusing it would only remake it).
 */
template <typename T>
class MemoryCache : public MemoryCacheBase {
public:

    // -- Types

    using key_type   = std::string;
    using value_type = std::shared_ptr<const T>;

    struct Statistics {
        size_t hits       = 0;
        size_t misses     = 0;
        size_t insertions = 0;
        size_t evictions  = 0;
    };

    // -- Constructors

    MemoryCache(MemoryCaches& caches, const MemoryUsage& capacity, const std::string& policy = Policy::DEFAULT) :
        MemoryCacheBase(caches), capacity_(capacity), policy_(Policy::build(policy)) {
        register_cache();
    }

    // -- Destructor

    ~MemoryCache() override { unregister_cache(); }

    // -- Methods

    /// Cached value, or make() after making room for the estimate; footprint is the value's footprint()
    template <typename Make>
    value_type get(const key_type& key, const MemoryUsage& estimate, const Make& make) {
        return get(key, estimate, make, [](const T& value) { return value.footprint(); });
    }

    /// As get, for values without footprint(); footprint(const T&) returns it
    template <typename Make, typename Footprint>
    value_type get(const key_type& key, const MemoryUsage& estimate, const Make& make, const Footprint& footprint) {
        // serialise requests per key, so the value is made once
        util::lock_guard<util::recursive_mutex> making(making_mutex(key));

        {
            util::lock_guard<util::recursive_mutex> lock(mutex_);

            if (auto it = entries_.find(key); it != entries_.end()) {
                ++statistics_.hits;
                it->second.info.last_use = ++clock_;
                ++it->second.info.uses;
                return it->second.value;
            }

            ++statistics_.misses;
            reserved_[key] = estimate;
            evict();
        }

        // make without holding the cache lock, as it can be slow
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
        const auto now = ++clock_;
        entries_.try_emplace(key, Entry{value, {footprint(*value), now, now, 1}});
        ++statistics_.insertions;
        evict();  // the footprint can differ from the estimate

        return value;
    }

    /// Release a value (if not in use elsewhere)
    void erase(const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        statistics_.evictions += entries_.erase(key);
    }

    MemoryUsage usage() const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        MemoryUsage sum;
        for (const auto& [key, entry] : entries_) {
            sum += entry.info.footprint;
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

    const Policy& policy() const { return *policy_; }

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

    // -- Overridden methods

    MemoryUsage footprint() const override { return usage(); }

    /// Values in use stay valid for their users
    void purge() override { clear(); }

private:

    // -- Types

    struct Entry {
        value_type value;
        Policy::Entry info;
    };

    // -- Members

    mutable util::recursive_mutex mutex_;
    std::map<key_type, Entry> entries_;
    std::map<key_type, MemoryUsage> reserved_;                           // estimates, of values being made
    std::map<key_type, std::unique_ptr<util::recursive_mutex>> making_;  // per key, never released (keys are few)
    MemoryUsage capacity_;
    std::unique_ptr<const Policy> policy_;
    Statistics statistics_;
    size_t clock_ = 0;

    // -- Methods

    util::recursive_mutex& making_mutex(const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        auto& m = making_[key];
        if (!m) {
            m = std::make_unique<util::recursive_mutex>();
        }
        return *m;
    }

    /// Evict values not in use (in policy order), until values and estimates of those being made fit
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

                // skip values in use (evicting doesn't release them) and those not relieving what is over
                const bool in_use   = entry.value.use_count() > 1;
                const bool relieves = (over.memory() > 0 && entry.info.footprint.memory() > 0) ||
                                      (over.shared() > 0 && entry.info.footprint.shared() > 0);

                if (!in_use && relieves &&
                    (victim == entries_.end() || policy_->before(entry.info, victim->second.info))) {
                    victim = it;
                }
            }

            if (victim == entries_.end()) {
                return;  // beyond capacity, nothing else to evict
            }

            usage -= victim->second.info.footprint;
            entries_.erase(victim);
            ++statistics_.evictions;
        }
    }
};


/**
 * @brief Values by key, made once and never evicted, returned by reference
 * @details For small values made often (e.g. Gaussian latitudes, pl arrays); footprint is footprint(), else size()
 * elements. Concurrent users can make a missing value more than once; the first inserted is kept. Purging invalidates
 * references held by users, so purge only when values are not in use.
 */
template <typename Key, typename Value>
class MemoryCacheT final : public MemoryCacheBase {
private:

    // -- Types

    template <typename V>
    using footprint_t = decltype(std::declval<V>().footprint());

    template <typename V, typename = std::void_t<>>
    struct has_footprint : std::false_type {};

    template <typename V>
    struct has_footprint<V, std::void_t<footprint_t<V>>> : std::true_type {};

public:

    // -- Types

    using key_type   = Key;
    using value_type = Value;

    // -- Constructors

    explicit MemoryCacheT(MemoryCaches& caches) : MemoryCacheBase(caches) { register_cache(); }

    // -- Destructor

    ~MemoryCacheT() override { unregister_cache(); }

    // -- Methods

    /// Cached value, or nullptr
    const value_type* find(const key_type& key) const {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        auto it = values_.find(key);
        return it == values_.end() ? nullptr : &it->second;
    }

    /// Cache a value unless already cached; @return the cached value
    const value_type& insert(const key_type& key, value_type&& value) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        return values_.try_emplace(key, std::move(value)).first->second;
    }

    // -- Overridden methods

    MemoryUsage footprint() const override {
        util::lock_guard<util::recursive_mutex> lock(mutex_);

        MemoryUsage sum;
        for (const auto& [key, value] : values_) {
            if constexpr (has_footprint<value_type>::value) {
                sum += value.footprint();
            }
            else {
                sum += MemoryUsage{value.size() * sizeof(typename value_type::value_type), 0};
            }
        }
        return sum;
    }

    /// Invalidates the references to the values
    void purge() override {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        values_.clear();
    }

private:

    // -- Members

    mutable util::recursive_mutex mutex_;
    std::map<key_type, value_type> values_;
};


}  // namespace eckit::cache
