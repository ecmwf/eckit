// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/maths/Functions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

size_t round(size_t x, size_t n) {
    return ((x + n - 1) / n) * n;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
