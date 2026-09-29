// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

/// Eckit literals provides user defined literals for IEC multi-byte units.
/// To make these literals available to your code use `using namespace eckit::literals`
/// Usage example:
/// ```
/// using namespace eckit::literals;
/// const auto bufferSize = 3_MiB;
/// ```
namespace eckit::literals {

/// Literal to express Kibibyte
constexpr std::uint64_t operator""_KiB(unsigned long long int x) {
    return 1024ULL * x;
}

/// Literal to express Mebibyte
constexpr std::uint64_t operator""_MiB(unsigned long long int x) {
    return 1024_KiB * x;
}

/// Literal to express Gibibyte
constexpr std::uint64_t operator""_GiB(unsigned long long int x) {
    return 1024_MiB * x;
}

/// Literal to express Tebibyte
constexpr std::uint64_t operator""_TiB(unsigned long long int x) {
    return 1024_GiB * x;
}

/// Literal to express Pebibyte
constexpr std::uint64_t operator""_PiB(unsigned long long int x) {
    return 1024_TiB * x;
}

/// Literal to express Exbibyte
constexpr std::uint64_t operator""_EiB(unsigned long long int x) {
    return 1024_PiB * x;
}
}  // namespace eckit::literals
