/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include <cstddef>
#include <functional>

namespace eckit {

/// Combine a hash value into a seed (boost::hash_combine equivalent).
///
/// Uses the 64-bit golden-ratio constant to mix bits and reduce
/// collisions when combining multiple hash values for use with
/// std::hash and unordered containers.
inline void hash_combine(std::size_t& seed, std::size_t value) noexcept {
    seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

/// Variadic convenience: combine multiple hash values into a seed.
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
    hash_combine(seed, std::hash<T>{}(v));
    (hash_combine(seed, std::hash<Rest>{}(rest)), ...);
}

}  // namespace eckit
