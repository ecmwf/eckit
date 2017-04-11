/*
 * (C) Copyright 1996-2017 ECMWF.
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
BloomFilter<T>::BloomFilter(size_t size) :
    size_(size),
    entries_(0),
    data_(elementCount(size), 0) {}


template <typename T>
BloomFilter<T>::~BloomFilter() {}


template <typename T>
bool BloomFilter<T>::empty() const {
    return (entries_ == 0);
}


template <typename T>
void BloomFilter<T>::insert(const T& value) {

    size_t bit_index = index(value);
    size_t elem = bit_index / sizeof(data_type);
    size_t offset = bit_index % sizeof(data_type);

    data_[elem] |= (data_type(1) << offset);
    entries_++;
}


template <typename T>
bool BloomFilter<T>::contains(const T& value) const {

    size_t bit_index = index(value);
    size_t elem = bit_index / sizeof(data_type);
    size_t offset = bit_index % sizeof(data_type);

    return !((data_[elem] & (data_type(1) << offset)) == 0);
}


template <typename T>
void BloomFilter<T>::print(std::ostream& s) const {
    s << "BloomFilter(size=" << size_ << ",entries=" << entries_ << ")";
}


template<typename T>
size_t BloomFilter<T>::elementCount(size_t nbits) {
    return (nbits == 0) ? 0 : ((nbits - 1) / sizeof(data_type)) + 1;
}


template<typename T>
size_t BloomFilter<T>::index(const T& value) const {

    // TODO: This involves creating an entirely unecessary std::string intermediate
    //       --> Can in principle be optimised.
    //       --> Either use a different hash function, or extract the digest in a
    //           non-string form (n.b. it always has the same size buffer...).
    //

    MD5 md5;
    md5.add(value);
    std::string digest = md5.digest();

    // An MD5 is a hexadecimal number. Take the modulus incrementally
    // Use an incremento of 2 bytes to avoid any possible negative overflows with 4-bytes and long.

    size_t idx = 0;
    for (size_t i = 0; i < digest.size(); i += 2) {
        std::string tmp(&digest[i], 2);
        idx = ((idx * 16 * 16) + strtol(tmp.c_str(), NULL, 16)) % size_;
    }

    return idx;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
