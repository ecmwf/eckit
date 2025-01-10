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

#include <map>
#include <numeric>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo {


class Cache {
public:
    using bytes_size_t = decltype(sizeof(int));

    static bytes_size_t total_footprint();
    static void total_purge();

protected:
    Cache();

private:
    virtual bytes_size_t footprint() const = 0;
    virtual void purge()                   = 0;
};


template <typename Key, typename Value>
class CacheT final : private Cache {
private:
    template <typename V>
    using footprint_t = decltype(std::declval<V>().footprint());

    template <typename V, typename = std::void_t<>>
    struct has_footprint : std::false_type {};

    template <typename V>
    struct has_footprint<V, std::void_t<footprint_t<V>>> : std::true_type {};

    template <typename V>
    static inline constexpr bool has_footprint_v = has_footprint<V>::value;

public:
    using key_type   = Key;
    using value_type = Value;

    CacheT() : mutex_(new util::recursive_mutex) { ASSERT(mutex_ != nullptr); }

    ~CacheT() { delete mutex_; }

    CacheT(const CacheT&) = delete;
    CacheT(CacheT&&)      = delete;

    CacheT& operator=(const CacheT&) = delete;
    CacheT& operator=(CacheT&&)      = delete;

    bool contains(const key_type& key) const {
        util::lock_guard<util::recursive_mutex> lock(*mutex_);
        return container_.find(key) != container_.end();
    }

    const value_type& operator[](const key_type& key) const {
        util::lock_guard<util::recursive_mutex> lock(*mutex_);
        return container_[key];
    }

    value_type& operator[](const key_type& key) {
        util::lock_guard<util::recursive_mutex> lock(*mutex_);
        return container_[key];
    }

    bytes_size_t footprint() const final {
        util::lock_guard<util::recursive_mutex> lock(*mutex_);
        return std::accumulate(container_.begin(), container_.end(), 0, [](bytes_size_t sum, const auto& kv) {
            if constexpr (has_footprint_v<value_type>) {
                return sum + kv.second.footprint();
            }
            else {
                return sum + kv.second.size() * sizeof(typename value_type::value_type);
            }
        });
    }

    void purge() final { container_.clear(); }

private:
    mutable std::map<key_type, value_type> container_;

    util::recursive_mutex* mutex_;
};


}  // namespace eckit::geo
