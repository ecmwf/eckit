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
 * Safe type casting between integer types, preserving the value across changes of width and signedness.
 */
#pragma once
#include <limits>
#include <string>
#include <type_traits>

#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace detail {

/// Tag defaulting the target type of 'into_signed'/'into_unsigned' to the same width as the source, so that an
/// explicit template argument names the target rather than being silently converted to before being checked.
struct deduce_target_type {};

/**
 *  Tells whether a value is representable by the target type, checking it against both the minimum and the
 *  maximum allowed value. Comparisons are only ever made in a type that represents both operands exactly, so
 *  that conversions mixing signed and unsigned types are decided correctly.
 *  @param value to check.
 *  @return whether the value can be cast to the target type without changing it.
 */
template <typename T, typename S, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<S>, int> = 0>
[[nodiscard]] constexpr bool fits(S value) {
    using L = std::numeric_limits<T>;

    if constexpr (std::is_signed_v<S> == std::is_signed_v<T>) {
        // sharing a signedness, the wider type represents both bounds and the value exactly
        using C = std::conditional_t<(sizeof(T) > sizeof(S)), T, S>;
        if constexpr (std::is_signed_v<C>) {
            return static_cast<C>(value) >= static_cast<C>(L::min()) &&
                   static_cast<C>(value) <= static_cast<C>(L::max());
        }
        else {
            return static_cast<C>(value) <= static_cast<C>(L::max());  // both start at zero
        }
    }
    else if constexpr (std::is_signed_v<S>) {
        return value >= 0 && static_cast<std::make_unsigned_t<S>>(value) <= L::max();
    }
    else {
        return value <= static_cast<std::make_unsigned_t<T>>(L::max());  // zero is always representable
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
 *  @throws BadCast if the value is negative, or too large for the target type.
 */
template <typename T = detail::deduce_target_type, typename S, std::enable_if_t<std::is_integral_v<S>, int> = 0>
[[nodiscard]] constexpr auto into_unsigned(S value) {
    if constexpr (std::is_same_v<T, detail::deduce_target_type>) {
        return into<std::make_unsigned_t<S>>(value);
    }
    else {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>,
                      "the explicit template argument of eckit::into_unsigned names the target type, "
                      "which must be an unsigned integer");
        return into<T>(value);
    }
}

/**
 *  Casts unsigned integer into signed.
 *  @param value to cast from.
 *  @return value cast into, same as before but signed type.
 *  @throws BadCast if the value is not representable by the target type.
 */
template <typename T = detail::deduce_target_type, typename S, std::enable_if_t<std::is_integral_v<S>, int> = 0>
[[nodiscard]] constexpr auto into_signed(S value) {
    if constexpr (std::is_same_v<T, detail::deduce_target_type>) {
        return into<std::make_signed_t<S>>(value);
    }
    else {
        static_assert(std::is_integral_v<T> && std::is_signed_v<T>,
                      "the explicit template argument of eckit::into_signed names the target type, "
                      "which must be a signed integer");
        return into<T>(value);
    }
}

}  // namespace eckit
