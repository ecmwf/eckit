/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   Oct 2012

#ifndef eckit_container_Cache_h
#define eckit_container_Cache_h

#include <sys/time.h>
#include <cstdint>

#include <iostream>
#include <map>
#include <utility>

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


/// @todo make an apply() method
/// @todo implement the expire() and the different policies

template <typename K, typename V>
class Cache : private NonCopyable {

public:  // types

    struct Entry {
        Entry(const V& v) : v_(v), expired_(false), hits_(0) {
            gettimeofday(&age_, 0);
            last_ = age_;
        }

        void reset(const V& v) {
            v_       = v;
            expired_ = false;
            hits_    = 0;
            gettimeofday(&age_, 0);
            last_ = age_;
        }

        V& access() {
            gettimeofday(&last_, 0);
            ++hits_;
            return v_;
        }

        V v_;
        bool expired_;
        uint64_t hits_;
        struct ::timeval age_;
        struct ::timeval last_;
    };

    typedef K key_type;
    typedef V value_type;
    typedef Entry entry_type;

    typedef std::map<key_type, entry_type> store_type;

    class Policy {
        /// Expires the Least Recently Used (LRU) entries
        /// @returns true if any entries were expired
        static bool expireLRU(store_type& c, const size_t& maxSize);
        /// Expires the Least Frequently Used (LFU) entries
        /// @returns true if any entries were expired
        static bool expireLFU(store_type& c, const size_t& maxSize);
        /// Expires the entries older than a certain age
        /// @returns true if any entries were expired
        static bool expireAge(store_type& c, struct ::timeval& maxAge);
    };

public:  // methods

    Cache();

    ~Cache();

    /// inserts an object in the cache
    /// @returns true if object was correctly inserted,
    ///          or false if key already existed and cannot be inserted
    bool insert(const K&, const V&);

    /// updates an object in the cache (or inserts if does not exist)
    /// @returns true if object existed and was updated,
    ///          false if the object did not exist and was inserted
    bool update(const K&, const V&);

    /// accesses an object in the cache
    /// @param v returns the object
    bool fetch(const K&, V&);

    /// marks an object as expired
    /// @returns true if object was present and is marked as expired
    bool expire(const K&);

    /// evicts entries that are considered expired
    void purge();

    /// evicts all entries
    void clear();

    /// @returns true if entry exists and is not expired in cache
    bool valid(const K&) const;

    /// @returns the number of entries in the cache, expired or not
    size_t size() const;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Cache& p) {
        p.print(s);
        return s;
    }

private:  // methods

    /// marks an object as expired
    /// @returns true if object was present and is marked as expired
    void expire(typename store_type::iterator i);

private:  // members

    store_type storage_;
};

//-----------------------------------------------------------------------------

template <typename K, typename V>
Cache<K, V>::Cache() : storage_() {}

template <typename K, typename V>
Cache<K, V>::~Cache() {
    clear();
}

template <typename K, typename V>
bool Cache<K, V>::insert(const K& k, const V& v) {
    typename store_type::iterator i = storage_.find(k);
    if (i != storage_.end()) {
        Entry& e = i->second;

        if (!e.expired_) {
            return false;
        }

        e.reset(v);
    }
    else {
        storage_.insert(std::make_pair(k, Entry(v)));
    }

    return true;
}

template <typename K, typename V>
bool Cache<K, V>::update(const K& k, const V& v) {
    typename store_type::iterator i = storage_.find(k);
    if (i != storage_.end()) {
        Entry& e = i->second;
        e.reset(v);
        return true;
    }
    storage_.insert(std::make_pair(k, Entry(v)));
    return false;
}

template <typename K, typename V>
bool Cache<K, V>::fetch(const K& k, V& v) {
    typename store_type::iterator i = storage_.find(k);
    if (i != storage_.end()) {
        Entry& e = i->second;
        if (!e.expired_) {
            v = e.access();
            return true;
        }
    }
    return false;
}

template <typename K, typename V>
bool Cache<K, V>::expire(const K& k) {
    typename store_type::iterator i = storage_.find(k);
    if (i != storage_.end()) {
        this->expire(i);
        return true;
    }
    return false;
}

template <typename K, typename V>
void Cache<K, V>::expire(typename store_type::iterator i) {
    Entry& e   = i->second;
    e.expired_ = true;
}

template <typename K, typename V>
void Cache<K, V>::purge() {
    // collect all expired
    typedef typename store_type::iterator siterator;
    std::vector<siterator> expired;
    for (siterator i = storage_.begin(); i != storage_.end(); ++i) {
        if (i->second.expired_) {
            expired.push_back(i);
        }
    }
    // remove them
    for (typename std::vector<siterator>::iterator e = expired.begin(); e != expired.end(); ++e) {
        storage_.erase(*e);
    }
}

template <typename K, typename V>
void Cache<K, V>::clear() {
    storage_.clear();
}

template <typename K, typename V>
bool Cache<K, V>::valid(const K& k) const {
    typename store_type::const_iterator i = storage_.find(k);
    if (i != storage_.end() && !i->second.expired_) {
        return true;
    }
    return false;
}

template <typename K, typename V>
size_t Cache<K, V>::size() const {
    return storage_.size();
}

template <typename K, typename V>
void Cache<K, V>::print(std::ostream& out) const {
    typedef typename store_type::const_iterator siterator;
    for (siterator i = storage_.begin(); i != storage_.end(); ++i) {
        out << i->second.v_ << std::endl;
    }
}

//-----------------------------------------------------------------------------

template <typename K, typename V>
bool Cache<K, V>::Policy::expireLRU(typename Cache<K, V>::store_type& c, const size_t& maxSize) {
    //    typedef std::vector< std::pair<
    //    typedef typename store_type::iterator siterator;
    //    for( siterator i = storage_.begin(); i != storage_.end(); ++i )

    //    bool found = false;
    //    typedef typename store_type::iterator siterator;
    //    for( siterator i = storage_.begin(); i != storage_.end(); ++i )
    //    {
    //        if( !i->second.expired_ && policy_->toExpire( i->second ) )
    //        {
    //            Entry& e = i->second;
    //            e.expired_ = true;
    //            found = true;
    //        }
    //    }
    //    return found;
    return false;
}

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
