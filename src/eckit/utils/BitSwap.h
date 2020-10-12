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
/// @date October 2020

#ifndef eckit_utils_BitSwap_H
#define eckit_utils_BitSwap_H

#include <cstdint>
#include <vector>
#include <algorithm>

#include "eckit/eckit.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// Low Level bitswap functions

unsigned char bitswap8(unsigned char a) {
  a = ((a * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32; // faster
  return a;
}

uint16_t bitswap16(uint16_t a) {
  a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
  return a;
}

uint32_t bitswap32(uint32_t a) {
  a = ((a & 0x000000FF) << 24) |
      ((a & 0x0000FF00) <<  8) |
      ((a & 0x00FF0000) >>  8) |
      ((a & 0xFF000000) >> 24);
  return a;
}

uint64_t bitswap64(uint64_t a) {
  a = ((a & 0x00000000000000FFULL) << 56) |
      ((a & 0x000000000000FF00ULL) << 40) |
      ((a & 0x0000000000FF0000ULL) << 24) |
      ((a & 0x00000000FF000000ULL) <<  8) |
      ((a & 0x000000FF00000000ULL) >>  8) |
      ((a & 0x0000FF0000000000ULL) >> 24) |
      ((a & 0x00FF000000000000ULL) >> 40) |
      ((a & 0xFF00000000000000ULL) >> 56);
  return a;
}

//----------------------------------------------------------------------------------------------------------------------

/// BitSwap selects the swap function based on data type size

template <int T> struct BitSwap {};

template <>
struct BitSwap<8> {
  typedef uint64_t inter_t;
  static uint64_t swap(uint64_t i) { return eckit::bitswap64(i); }
};

template <>
struct BitSwap<4> {
  typedef uint32_t inter_t;
  static uint32_t swap(uint32_t i) { return eckit::bitswap32(i); }
};

template <>
struct BitSwap<2> {
  typedef uint16_t inter_t;
  static uint16_t swap(uint16_t i) { return eckit::bitswap16(i); }
};

template <>
struct BitSwap<1> {
  typedef unsigned char inter_t;
  static unsigned char swap(unsigned char i) { return eckit::bitswap8(i); }
};

//----------------------------------------------------------------------------------------------------------------------

/// Scalar bitswap template function uses BitSwap to select bsed on type T size

template <typename T> T bitswap(T i) {
  constexpr std::size_t sz = sizeof(T);
  typename BitSwap<sz>::inter_t* v = reinterpret_cast<typename BitSwap<sz>::inter_t*>(&i);
  typename BitSwap<sz>::inter_t  r = BitSwap<sz>::swap(*v);
  return * reinterpret_cast<typename BitSwap<sz>::inter_t*>(&r);
}

/// std::vector bitswap template function uses BitSwap to select bsed on type T size

template <typename T> void bitswap(std::vector<T>& vi) {
  constexpr std::size_t sz = sizeof(T);
  typename BitSwap<sz>::inter_t* vt = reinterpret_cast<typename BitSwap<sz>::inter_t*>(vi.data());
  std::transform(vt, vt + vi.size(), vt,
                   [](typename BitSwap<sz>::inter_t& e) -> typename BitSwap<sz>::inter_t { return eckit::bitswap(e); });
}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
