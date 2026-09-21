// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_EnumBitmasks_H
#define eckit_utils_EnumBitmasks_H

#include <type_traits>

namespace eckit {
#define ENUM_FLAG_OPERATORS(T)                                                          \
    inline constexpr T operator&(T X, T Y) {                                            \
        return static_cast<T>(static_cast<typename std::underlying_type<T>::type>(X) &  \
                              static_cast<typename std::underlying_type<T>::type>(Y));  \
    }                                                                                   \
    inline constexpr T operator|(T X, T Y) {                                            \
        return static_cast<T>(static_cast<typename std::underlying_type<T>::type>(X) |  \
                              static_cast<typename std::underlying_type<T>::type>(Y));  \
    }                                                                                   \
    inline constexpr T operator^(T X, T Y) {                                            \
        return static_cast<T>(static_cast<typename std::underlying_type<T>::type>(X) ^  \
                              static_cast<typename std::underlying_type<T>::type>(Y));  \
    }                                                                                   \
    inline constexpr T operator~(T X) {                                                 \
        return static_cast<T>(~static_cast<typename std::underlying_type<T>::type>(X)); \
    }                                                                                   \
    inline T& operator&=(T& X, T Y) {                                                   \
        X = X & Y;                                                                      \
        return X;                                                                       \
    }                                                                                   \
    inline T& operator|=(T& X, T Y) {                                                   \
        X = X | Y;                                                                      \
        return X;                                                                       \
    }                                                                                   \
    inline T& operator^=(T& X, T Y) {                                                   \
        X = X ^ Y;                                                                      \
        return X;                                                                       \
    }


}  // end namespace eckit

#endif
