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
/// @date   Dec 2015

#ifndef eckit_container_CacheLRU_h
#define eckit_container_CacheLRU_h

#include <iosfwd>
#include <list>
#include <map>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename K, typename V>
class CacheLRU : private NonCopyable {

public:  // types

    typedef K key_type;
    typedef V value_type;

    struct Entry {

        key_type key_;
        value_type value_;

        Entry(const key_type& k, const value_type& v) : key_(k), value_(v) {}

        friend std::ostream& operator<<(std::ostream& s, const Entry& e) {
            s << "key=" << e.key_;
            return s;
        }
    };

    typedef Entry entry_type;

    typedef std::list<entry_type> storage_type;
    typedef typename storage_type::iterator storage_iterator;
    typedef std::map<key_type, storage_iterator> map_type;

    typedef void (*purge_handler_type)(key_type&, value_type&);

public:  // methods

    CacheLRU(size_t capacity, purge_handler_type purge = 0);

    ~CacheLRU();

    /// Inserts an entry into the cache, overwrites if already exists
    /// @returns true if a key already existed
    bool insert(const key_type& key, const value_type& value);

    /// Accesses a key that must already exist
    /// @throws OutOfRange exception is key not in cache
    value_type access(const key_type& key);

    /// Extracts the key from the cache without purging
    /// @pre Key must exist in cache
    /// @throws OutOfRange exception if key not in cache
    value_type extract(const key_type& key);

    /// Remove a key-value pair from the cache
    /// No effect if key is not present
    ///
    /// @return true if removed
    bool remove(const key_type& key);

    /// @returns true if the key exists in the cache
    bool exists(const key_type& key) const;

    /// Clears all entries in the cache
    void clear();

    /// @returns the maximum size of the cache
    size_t capacity() const { return capacity_; }

    /// @returns the current (used) size of the cache
    size_t size() const { return storage_.size(); }

    /// resizes the cache capacity
    void capacity(size_t size);

    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& s, const CacheLRU& p) {
        p.print(s);
        return s;
    }

private:  // methods

    void erase(typename map_type::iterator itr);

    void trim();

    void moveToFront(typename map_type::iterator itr);

    value_type& valueFrom(typename map_type::iterator itr) const { return itr->second->value_; }

    void purge(key_type& key, value_type& value) const;

private:  // members

    storage_type storage_;

    map_type map_;

    size_t capacity_;

    purge_handler_type purge_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "CacheLRU.cc"

#endif
