/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/**
 * Place functions to allow for save type casting in this file
 */
#pragma once
#include <limits>
#include <string>
#include <type_traits>

#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace detail {

/// Tag defaulting the first template parameter of 'into_signed'/'into_unsigned', so that supplying an
/// explicit template argument (which would name the *source* type) is rejected instead of silently
/// converting the argument before the check is applied.
struct deduce_source_type {};

/**
 *  Tells whether a value is representable by the target type, checking it against both the minimum and the
 *  maximum allowed value. Comparisons are only ever made in a type that represents both operands exactly, so
 *  that conversions mixing signed and unsigned types are decided correctly.
 *  @param value to check.
 *  @return whether the value can be cast to the target type without changing it.
 */
template <typename T, typename S, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<S>, int> = 0>
[[nodiscard]] constexpr bool fits(S value) {
    if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) {
        // same signedness: a target at least as wide represents every value exactly
        if constexpr (sizeof(T) >= sizeof(S)) {
            return true;
        }
        else if constexpr (std::is_signed_v<S>) {
            return value >= static_cast<S>(std::numeric_limits<T>::min()) &&
                   value <= static_cast<S>(std::numeric_limits<T>::max());
        }
        else {
            // the minimum (0) is shared by both types, so only the maximum can be exceeded
            return value <= static_cast<S>(std::numeric_limits<T>::max());
        }
    }
    else if constexpr (std::is_signed_v<S>) {
        // signed into unsigned: the minimum check rejects every negative value
        if (value < 0) {
            return false;
        }
        if constexpr (sizeof(T) >= sizeof(S)) {
            return true;
        }
        else {
            using US = std::make_unsigned_t<S>;
            return static_cast<US>(value) <= static_cast<US>(std::numeric_limits<T>::max());
        }
    }
    else {
        // unsigned into signed: the minimum (0) is always representable, a strictly wider target always fits
        if constexpr (sizeof(T) > sizeof(S)) {
            return true;
        }
        else {
            return value <= static_cast<S>(std::numeric_limits<T>::max());
        }
    }
}

}  // namespace detail

/**
 *  Casts an integer into another integer type, checking that the value is preserved. Both the minimum and the
 *  maximum allowed value are checked, covering conversions that mix signed and unsigned types as well as
 *  conversions that narrow (such as 'long long' into 'long', or 'long' into 'int').
 *  @param value to cast from.
 *  @return value cast into the requested type.
 *  @throws BadCast if the value is not representable by the target type.
 */
template <typename T, typename S, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<S>, int> = 0>
[[nodiscard]] constexpr T into(S value) {
    if (!detail::fits<T>(value)) {
        throw eckit::BadCast("Value " + std::to_string(value) + " cannot be cast to the target type", Here());
    }
    return static_cast<T>(value);
}

/**
 *  Casts signed integer into unsigned.
 *  @param value to cast from.
 *  @return value cast into, same as before but unsigned type.
 *  @throws BadCast if used with a negative value.
 */
template <typename Deduce = detail::deduce_source_type, typename S,
          std::enable_if_t<std::is_integral_v<S> && !std::is_same_v<std::remove_cv_t<S>, bool>, int> = 0>
[[nodiscard]] constexpr auto into_unsigned(S value) -> std::make_unsigned_t<S> {
    static_assert(std::is_same_v<Deduce, detail::deduce_source_type>,
                  "eckit::into_unsigned takes no explicit template argument: it would name the source type, "
                  "converting the argument before it is checked. Use eckit::into<T>(value) to pick a target type.");
    return into<std::make_unsigned_t<S>>(value);
}

/**
 *  Casts unsigned integer into signed.
 *  @param value to cast from.
 *  @return value cast into, same as before but signed type.
 *  @throws BadCast if used with a value > 2^(bits-1)-1
 */
template <typename Deduce = detail::deduce_source_type, typename S,
          std::enable_if_t<std::is_integral_v<S> && !std::is_same_v<std::remove_cv_t<S>, bool>, int> = 0>
[[nodiscard]] constexpr auto into_signed(S value) -> std::make_signed_t<S> {
    static_assert(std::is_same_v<Deduce, detail::deduce_source_type>,
                  "eckit::into_signed takes no explicit template argument: it would name the source type, "
                  "converting the argument before it is checked. Use eckit::into<T>(value) to pick a target type.");
    return into<std::make_signed_t<S>>(value);
}

}  // namespace eckit
