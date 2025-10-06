/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date October 2020

#ifndef eckit_utils_ByteSwap_H
#define eckit_utils_ByteSwap_H

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

#include "eckit/eckit.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// Low Level bitswap functions

inline uint16_t bitswap16(uint16_t a) {
    a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
    return a;
}

inline uint32_t bitswap32(uint32_t a) {
    a = ((a & 0x000000FF) << 24) | ((a & 0x0000FF00) << 8) | ((a & 0x00FF0000) >> 8) | ((a & 0xFF000000) >> 24);
    return a;
}

inline uint64_t bitswap64(uint64_t a) {
    a = ((a & 0x00000000000000FFULL) << 56) | ((a & 0x000000000000FF00ULL) << 40) |
        ((a & 0x0000000000FF0000ULL) << 24) | ((a & 0x00000000FF000000ULL) << 8) | ((a & 0x000000FF00000000ULL) >> 8) |
        ((a & 0x0000FF0000000000ULL) >> 24) | ((a & 0x00FF000000000000ULL) >> 40) | ((a & 0xFF00000000000000ULL) >> 56);
    return a;
}

//----------------------------------------------------------------------------------------------------------------------

/// ByteSwap selects the swap function based on data type size

template <int T>
struct ByteSwap {};

template <>
struct ByteSwap<8> {
    typedef uint64_t inter_t;
    static void bitswap(uint64_t& i) { i = eckit::bitswap64(i); }
};

template <>
struct ByteSwap<4> {
    typedef uint32_t inter_t;
    static void bitswap(uint32_t& i) { i = eckit::bitswap32(i); }
};

template <>
struct ByteSwap<2> {
    typedef uint16_t inter_t;
    static void bitswap(uint16_t& i) { i = eckit::bitswap16(i); }
};

//----------------------------------------------------------------------------------------------------------------------

/// Scalar bitswap template function uses ByteSwap to select based on type T size

template <typename T>
void byteswap(T& i) {
    constexpr std::size_t sz = sizeof(T);
    // std::cerr << "byteswap(T) sizeof T = " << sz << std::endl;
    typename ByteSwap<sz>::inter_t* v = reinterpret_cast<typename ByteSwap<sz>::inter_t*>(&i);
    ByteSwap<sz>::bitswap(*v);
}

/// C-array bitswap template function uses ByteSwap to select based on type T size

template <typename T>
void byteswap(T data[], size_t size) {
    constexpr std::size_t sz = sizeof(T);
    // std::cerr << "array byteswap(T) sizeof T = " << sz << std::endl;
    auto vt = reinterpret_cast<typename ByteSwap<sz>::inter_t*>(data);
    std::for_each(vt, vt + size, [](typename ByteSwap<sz>::inter_t& e) { eckit::byteswap(e); });
}

/// std::vector bitswap template function uses ByteSwap to select based on type T size

template <typename T>
void byteswap(std::vector<T>& vi) {
    eckit::byteswap<T>(vi.data(), vi.size());
}

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
std::string bits_to_str(T v) {
    constexpr std::size_t sz          = sizeof(T);
    typename ByteSwap<sz>::inter_t* t = reinterpret_cast<typename ByteSwap<sz>::inter_t*>(&v);
    constexpr std::size_t nbits       = 8 * sizeof(T);
    std::bitset<nbits> bits(*t);
    return bits.to_string();
}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
