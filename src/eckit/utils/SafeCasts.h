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
#include <type_traits>

#include "eckit/exception/Exceptions.h"

namespace eckit {

/**
 *  Casts signed integer into unsigned.
 *  @param value to cast from.
 *  @return value cast into, same as before but unsigned type.
 *  @throws BadCast if used with a negative value.
 */
template <typename S, std::enable_if_t<std::is_integral_v<S> && std::is_signed_v<S>, int> = 0>
[[nodiscard]] constexpr auto into_unsigned(S value) -> std::make_unsigned_t<S> {
    using U = std::make_unsigned_t<S>;
    if (value < 0) {
        throw eckit::BadCast("Negative value cannot be cast to unsigned type", Here());
    }
    return static_cast<U>(value);
}

/**
 * Template specialization that turns unsigned to unsigned conversions with 'into_unsigned' into a NoOp
 * @param value
 * @return value, returned unmodified.
 */
template <typename S, std::enable_if_t<std::is_integral_v<S> && std::is_unsigned_v<S>, int> = 0>
[[nodiscard]] constexpr auto into_unsigned(S value) -> std::make_unsigned_t<S> {
    return value;
}

/**
 *  Casts unsigned integer into signed.
 *  @param value to cast from.
 *  @return value cast into, same as before but signed type.
 *  @throws BadCast if used with a value > 2^(bits-1)-1
 */
template <typename U, std::enable_if_t<std::is_integral_v<U> && std::is_unsigned_v<U>, int> = 0>
[[nodiscard]] constexpr auto into_signed(U value) -> std::make_signed_t<U> {
    using S = std::make_signed_t<U>;
    if (value > static_cast<U>(std::numeric_limits<S>::max())) {
        throw eckit::BadCast("Value too large to cast to signed type");
    }
    return static_cast<S>(value);
}

/**
 * Template specialization that turns signed to signed conversions with 'into_signed' into a NoOp
 * @param value
 * @return value, returned unmodified.
 */
template <typename U, std::enable_if_t<std::is_integral_v<U> && std::is_signed_v<U>, int> = 0>
[[nodiscard]] constexpr auto into_signed(U value) -> std::make_signed_t<U> {
    return value;
}

}  // namespace eckit
