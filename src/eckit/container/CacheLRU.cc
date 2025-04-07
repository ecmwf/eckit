/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/container/CacheLRU.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename K, typename V>
CacheLRU<K, V>::CacheLRU(size_t capacity, purge_handler_type purge) : capacity_(capacity), purge_(purge) {}

template <typename K, typename V>
CacheLRU<K, V>::~CacheLRU() {
    clear();
}

template <typename K, typename V>
bool CacheLRU<K, V>::insert(const key_type& key, const value_type& value) {
    bool existed = false;

    typename map_type::iterator itr = map_.find(key);
    if (itr != map_.end()) {
        existed = true;

        // erase the key from where it is
        // we'll reinsert it again so it comes on top

        erase(itr);
    }

    storage_.push_front(Entry(key, value));
    map_[key] = storage_.begin();

    trim();

    return existed;
}

template <typename K, typename V>
V CacheLRU<K, V>::access(const key_type& key) {
    // check first the front() since it is the most popular/recent entry

    if (size() && storage_.front().key_ == key)
        return storage_.front().value_;

    typename map_type::iterator itr = map_.find(key);

    if (itr != map_.end()) {
        // move entry of list to front
        // this keeps the most popular in front

        moveToFront(itr);

        return valueFrom(itr);
    }
    else {
        throw eckit::OutOfRange("key not in CacheLRU", Here());
    }
}

template <typename K, typename V>
V CacheLRU<K, V>::extract(const key_type& key) {
    typename map_type::iterator itr = map_.find(key);
    if (itr == map_.end()) {
        throw OutOfRange("key not in CacheLRU", Here());
    }

    value_type result = valueFrom(itr);
    erase(itr);

    return result;
}

template <typename K, typename V>
bool CacheLRU<K, V>::remove(const key_type& key) {
    bool existed = false;

    typename map_type::iterator itr = map_.find(key);
    if (itr != map_.end()) {
        existed = true;
        purge(itr->second->key_, valueFrom(itr));
        erase(itr);
    }

    return existed;
}

template <typename K, typename V>
bool CacheLRU<K, V>::exists(const key_type& key) const {
    return (map_.find(key) != map_.end());
}

template <typename K, typename V>
void CacheLRU<K, V>::clear() {
    for (storage_iterator itr = storage_.begin(); itr != storage_.end(); ++itr) {
        purge(itr->key_, itr->value_);
    }

    storage_.clear();
    map_.clear();
}

template <typename K, typename V>
void CacheLRU<K, V>::erase(typename map_type::iterator itr) {
    storage_.erase(itr->second);
    map_.erase(itr);
}

template <typename K, typename V>
void CacheLRU<K, V>::trim() {
    while (map_.size() > capacity_) {
        entry_type entry = storage_.back();
        purge(entry.key_, entry.value_);
        map_.erase(entry.key_);
        storage_.pop_back();
    }
}

template <typename K, typename V>
void CacheLRU<K, V>::moveToFront(typename map_type::iterator itr) {
    storage_.splice(storage_.begin(), storage_, itr->second);
}

template <typename K, typename V>
void CacheLRU<K, V>::purge(key_type& key, value_type& value) const {
    if (purge_)
        purge_(key, value);
}

template <typename K, typename V>
void CacheLRU<K, V>::capacity(size_t size) {
    capacity_ = size;
    trim();
}

template <typename K, typename V>
void CacheLRU<K, V>::print(std::ostream& os) const {
    os << "CacheLRU(capacity=" << capacity_ << ",size=" << storage_.size() << ",storage={";
    for (typename storage_type::const_iterator itr = storage_.begin(); itr != storage_.end(); ++itr) {
        os << *itr << ",";
    }

    os << "})";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
