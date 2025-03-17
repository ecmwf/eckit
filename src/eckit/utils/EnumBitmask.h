/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_EnumBitmasks_H
#define eckit_utils_EnumBitmasks_H

namespace eckit {

#include <type_traits>
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
