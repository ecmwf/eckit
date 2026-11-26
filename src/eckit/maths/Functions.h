// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_Functions_h
#define eckit_Functions_h

#include <cctype>
#include <cstdlib>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @returns rounds x to multiple of n
size_t round(size_t x, size_t n);

/// @returns the sign of ordereable types that support the subtraction operator
template <typename T>
int sign(T v) {
    return (T(0) < v) - (v < T(0));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
