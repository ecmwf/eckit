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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo::util {


class Cache {
public:
    using bytes_t = decltype(sizeof(int));

    static bytes_t total_footprint();
    static void total_purge();

protected:
    Cache();

private:
    virtual bytes_t footprint() const = 0;
    virtual void purge()              = 0;
};


template <typename Key, typename Value>
class CacheT final : private Cache {
public:
    using key_type   = Key;
    using value_type = Value;

    CacheT() :
        mutex_(new recursive_mutex) {
        ASSERT(mutex_ != nullptr);
    }

    bool contains(const key_type& key) const {
        lock_guard<recursive_mutex> lock(*mutex_);
        return container_.find(key) != container_.end();
    }

    const value_type& operator[](const key_type& key) const {
        lock_guard<recursive_mutex> lock(*mutex_);
        return container_[key];
    }

    value_type& operator[](const key_type& key) {
        lock_guard<recursive_mutex> lock(*mutex_);
        return container_[key];
    }

    bytes_t footprint() const final {
        lock_guard<recursive_mutex> lock(*mutex_);
        return std::accumulate(container_.begin(), container_.end(), 0, [](bytes_t sum, const auto& kv) {
            return sum + kv.second.size() * sizeof(typename value_type::value_type);
        });
    }

    void purge() final { container_.clear(); }

private:
    mutable std::map<Key, Value> container_;

    recursive_mutex* mutex_;
};


}  // namespace eckit::geo::util
