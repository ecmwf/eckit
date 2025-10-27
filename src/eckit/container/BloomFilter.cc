/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/container/BloomFilter.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"

#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


template <typename T>
BloomFilter<T>::BloomFilter(size_t size) : size_(size), entries_(0), data_(elementCount(size), 0) {}


template <typename T>
BloomFilter<T>::~BloomFilter() {}


template <typename T>
bool BloomFilter<T>::empty() const {
    return (entries_ == 0);
}


template <typename T>
void BloomFilter<T>::insert(const T& value) {

    size_t bit_index = index(value);
    size_t elem      = bit_index / sizeof(data_type);
    size_t offset    = bit_index % sizeof(data_type);

    data_[elem] |= (data_type(1) << offset);
    entries_++;
}


template <typename T>
bool BloomFilter<T>::contains(const T& value) const {

    size_t bit_index = index(value);
    size_t elem      = bit_index / sizeof(data_type);
    size_t offset    = bit_index % sizeof(data_type);

    return !((data_[elem] & (data_type(1) << offset)) == 0);
}


template <typename T>
void BloomFilter<T>::print(std::ostream& s) const {
    s << "BloomFilter(size=" << size_ << ",entries=" << entries_ << ")";
}


template <typename T>
size_t BloomFilter<T>::elementCount(size_t nbits) {
    return (nbits == 0) ? 0 : ((nbits - 1) / sizeof(data_type)) + 1;
}


template <typename T>
size_t BloomFilter<T>::index(const T& value) const {

    // n.b. We could use any other hash, but MD5 is already in here.

    MD5 md5;
    unsigned char md5_buf[16];
    md5.add(value);
    md5.numericalDigest(md5_buf);

    // Take the modules incrementally

    size_t idx = 0;
    for (size_t i = 0; i < sizeof(md5_buf); i++)
        idx = ((idx * 256) + md5_buf[i]) % size_;

    return idx;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
