// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file trie.h
/// @author Baudouin Raoult
/// @author Simon Smart
/// @date March 2017

#ifndef eckit_containers_BloomFilter_H
#define eckit_containers_BloomFilter_H

#include <ostream>
#include <vector>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
class BloomFilter {

public:  // types

    using data_type = unsigned long long;

public:  // methods

    BloomFilter(size_t size);

    BloomFilter(const BloomFilter&)            = delete;
    BloomFilter& operator=(const BloomFilter&) = delete;
    BloomFilter(BloomFilter&&)                 = delete;
    BloomFilter& operator=(BloomFilter&&)      = delete;

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
