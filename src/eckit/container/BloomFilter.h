/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file trie.h
/// @author Baudouin Raoult
/// @author Simon Smart
/// @date March 2017


#ifndef eckit_containers_BloomFilter_H
#define eckit_containers_BloomFilter_H

#include "eckit/memory/NonCopyable.h"

#include <ostream>
#include <vector>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
class BloomFilter : private NonCopyable {

public:  // types

    typedef unsigned long long data_type;

public:  // methods

    BloomFilter(size_t size);
    ~BloomFilter();

    bool empty() const;
    void insert(const T& value);
    bool contains(const T& value) const;

protected:  // methods

    void print(std::ostream&) const;

private:  // members

    static size_t elementCount(size_t nbits);

    /// Which bit should we be considering?
    size_t index(const T& value) const;

private:  // members

    size_t size_;
    size_t entries_;

    // n.b. We don't use std::vector<bool>, as it doesn't behave like a std::vector.
    //      Better to be explicit

    std::vector<data_type> data_;

private:  // friends

    friend std::ostream& operator<<(std::ostream& s, const BloomFilter<T>& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#include "BloomFilter.cc"

#endif  // eckit_containers_BloomFilter_H
