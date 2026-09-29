// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_Clock_h
#define eckit_utils_Clock_h

#include "time.h"

namespace eckit {

class Clock {
public:

    /// @returns UNIX Time since Epoch
    static time_t now() { return ::time(nullptr); }
};

}  // namespace eckit

#endif
