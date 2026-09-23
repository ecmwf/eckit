// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/eckit_config.h"

#ifndef eckit_BIG_ENDIAN
#error eckit_BIG_ENDIAN not defined
#endif

#ifndef eckit_LITTLE_ENDIAN
#error eckit_LITTLE_ENDIAN not defined
#endif

namespace eckit::codec {

enum class Endian {
    little = 0,
    big    = 1,
#if eckit_BIG_ENDIAN
    native  = big,
    swapped = little
#elif eckit_LITTLE_ENDIAN
    native  = little,
    swapped = big
#else
#error Neither eckit_BIG_ENDIAN nor eckit_LITTLE_ENDIAN equals true
#endif
};


}  // namespace eckit::codec
