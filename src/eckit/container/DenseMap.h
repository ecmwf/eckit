/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_DenseMap_h
#define eckit_DenseMap_h

/// @author Tiago Quintino

#include <algorithm>
#include <deque>
#include <utility>

#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <typename K, typename V>
class DenseMap {
public:  // types

    typedef K key_type;    ///< key type
    typedef V value_type;  ///< value type

    typedef std::pair<K, V> item_type;  ///< (key, value) item type

private:  // types

    typedef std::deque<item_type> store_t;

public:  // methods

    typedef typename store_t::iterator iterator;
    typedef typename store_t::const_iterator const_iterator;

    DenseMap() : sorted_(true) {}

    ~DenseMap() {}

    void insert(const K& k, const V& v) {
        items_.push_back(std::make_pair(k, v));
        sorted_ = false;
    }

    /// @TODO shoudl we implement this?
    //	size_t erase( const key_type& k );

    void replace(const K& k, const V& v) {
        iterator it = find(k);
        if (it != end()) {
            it->second = v;
        }
        else {
            insert(k, v);
        }
    }

    void clear() {
        items_.clear();
        sorted_ = true;
    }

    bool sorted() const { return sorted_; }

    size_t size() const {
        ASSERT(sorted_);
        return items_.size();
    }
    bool empty() const { return items_.empty(); }

    void sort() {
        if (sorted_) {
            return;
        }

        std::stable_sort(items_.begin(), items_.end(), LessThan());
        auto last     = std::unique(items_.rbegin(), items_.rend(), Equals());
        auto position = std::distance(last, items_.rend());
        items_.erase(items_.begin(), items_.begin() + position);
        sorted_ = true;
    }

    iterator begin() { return items_.begin(); }
    const_iterator begin() const { return items_.begin(); }
    const_iterator cbegin() const { return items_.cbegin(); }

    iterator end() { return items_.end(); }
    const_iterator end() const { return items_.end(); }
    const_iterator cend() const { return items_.cend(); }

    bool contains(const K& k) const { return find(k) != cend(); }

    const value_type& get(const K& k) const { return find(k)->second; }
    value_type& get(const K& k) { return find(k)->second; }

    const value_type& at(const size_t i) const {
        ASSERT(i < items_.size());
        return items_[i].second;
    }
    value_type& at(const size_t i) {
        ASSERT(i < items_.size());
        return items_[i].second;
    }

    const value_type& operator[](const K& k) const { return find(k)->second; }
    value_type& operator[](const K& k) { return find(k)->second; }

    const value_type& operator[](const size_t& i) const { return items_[i].second; }
    value_type& operator[](const size_t& i) { return items_[i].second; }

    iterator find(const K& k) {
        if (empty()) {
            return end();
        }

        ASSERT(sorted());
        iterator it = std::lower_bound(begin(), end(), k, Compare());
        if (it != end() && it->first == k) {
            return it;
        }

        return end();
    }

    const_iterator find(const K& k) const {
        if (empty()) {
            return cend();
        }

        ASSERT(sorted());
        const_iterator it = std::lower_bound(cbegin(), cend(), k, Compare());
        if (it != cend() && it->first == k) {
            return it;
        }

        return cend();
    }

    void print(std::ostream& s) const {
        const_iterator it = cbegin();
        for (; it != cend(); ++it) {
            s << it->first << " " << it->second << std::endl;
        }
    }

    friend std::ostream& operator<<(std::ostream& s, const DenseMap& m) {
        m.print(s);
        return s;
    }

private:  // types

    class LessThan {
    public:

        bool operator()(const item_type& e1, const item_type& e2) const { return (e1.first < e2.first) ? true : false; }
    };

    class Equals {
    public:

        bool operator()(const item_type& e1, const item_type& e2) const {
            return (e1.first == e2.first) ? true : false;
        }
    };

    class Compare {
    public:

        bool operator()(const item_type& e, const K& k) const { return (e.first < k) ? true : false; }
        bool operator()(const K& k, const item_type& e) const { return (e.first > k) ? true : false; }
    };

private:  // members

    store_t items_;  ///< storage of the items

    bool sorted_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
