// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Oct 2016

#ifndef eckit_AutoUmask_h
#define eckit_AutoUmask_h

#include <sys/stat.h>


namespace eckit {


class AutoUmask {
    mode_t umask_;

public:

    explicit AutoUmask(mode_t u = 0) : umask_(::umask(u)) {}
    ~AutoUmask() { ::umask(umask_); }
};


}  // namespace eckit

#endif
