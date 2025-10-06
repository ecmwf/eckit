/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_DenseSet_h
#define eckit_DenseSet_h

/// @author Tiago Quintino
/// @author Olivier Iffrig

#include <algorithm>
#include <utility>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/JSON.h"


namespace eckit {


template <typename V>
class DenseSet {
public:  // types

    typedef V value_type;  ///< value type

private:  // types

    typedef std::vector<value_type> store_t;

public:  // methods

    typedef typename store_t::const_reference const_reference;

    typedef typename store_t::const_iterator iterator;
    typedef typename store_t::const_iterator const_iterator;

    DenseSet(size_t s = 0) : sorted_(true) {
        if (s > 0) {
            reserve(s);
        }
    }

    ~DenseSet() {}

    void reserve(size_t s) { values_.reserve(s); }

    void insert(const V& v) {
        if (!values_.empty() && v <= values_.back()) {
            sorted_ = false;
        }
        values_.push_back(v);
    }

    /// @TODO shoudl we implement this?
    //    size_t erase( const value_type& v );

    void clear() {
        values_.clear();
        sorted_ = true;
    }

    bool sorted() const { return sorted_; }

    size_t size() const { return values_.size(); }
    bool empty() const { return values_.empty(); }

    void sort() {
        if (!sorted_) {
            std::sort(values_.begin(), values_.end());
            auto last = std::unique(values_.begin(), values_.end());
            values_.erase(last, values_.end());
            sorted_ = true;
        }
    }

    iterator begin() { return values_.begin(); }
    const_iterator begin() const { return values_.begin(); }
    const_iterator cbegin() const { return values_.cbegin(); }

    iterator end() { return values_.end(); }
    const_iterator end() const { return values_.end(); }
    const_iterator cend() const { return values_.cend(); }

    bool contains(const V& v) const { return find(v) != cend(); }

    const_reference at(const size_t i) const {
        ASSERT(i < values_.size());
        return values_[i];
    }

    const_reference operator[](const size_t& i) const { return values_[i]; }

    iterator find(const V& v) {
        if (empty()) {
            return end();
        }

        ASSERT(sorted());
        iterator it = std::lower_bound(begin(), end(), v);
        if (it != end() && *it == v) {
            return it;
        }

        return end();
    }

    const_iterator find(const V& v) const {
        if (empty()) {
            return cend();
        }

        ASSERT(sorted());
        const_iterator it = std::lower_bound(cbegin(), cend(), v);
        if (it != cend() && *it == v) {
            return it;
        }

        return cend();
    }

    void print(std::ostream& s) const {
        const_iterator it = cbegin();
        for (; it != cend(); ++it) {
            s << *it << std::endl;
        }
    }

    void json(JSON& s) const {
        s.startList();
        const_iterator it = cbegin();
        for (; it != cend(); ++it) {
            s << *it;
        }
        s.endList();
    }

    bool operator==(const DenseSet& rhs) const { return !operator!=(rhs); }

    bool operator!=(const DenseSet& rhs) const { return values_ != rhs.values_; }

    friend std::ostream& operator<<(std::ostream& s, const DenseSet& m) {
        m.print(s);
        return s;
    }

    friend JSON& operator<<(JSON& s, const DenseSet& m) {
        m.json(s);
        return s;
    }

    void merge(const DenseSet& other) {

        if (other.empty())
            return;

        if (empty()) {
            values_ = other.values_;
            sorted_ = other.sorted_;
            return;
        }

        ASSERT(sorted_);
        ASSERT(other.sorted_);

        int i = 0;
        int j = 0;

        // n.b. resizes would invalidate iterators --> use indices instead

        while (j < other.values_.size()) {

            if (i >= values_.size() || values_[i] > other.values_[j]) {
                if (values_.capacity() == values_.size()) {
                    values_.reserve(std::max(values_.size(), other.values_.size()) * 2);
                }
                values_.insert(values_.begin() + i, other.values_[j]);
                i++;
                j++;
            }
            else if (values_[i] == other.values_[j]) {
                j++;
            }
            else {  // if (*it1 < *it2)
                i++;
            }
        }
    }

private:  // members

    store_t values_;  ///< storage of the values

    bool sorted_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
