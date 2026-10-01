// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_os_Stat_h
#define eckit_os_Stat_h

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "eckit/eckit.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// struct to manage differences between stat and stat64 different OS's
struct Stat {
    /// prefer using stat if supports 64 bit

    using Struct = struct stat;

    static int stat(const char* path, Struct* s) { return ::stat(path, s); }
    static int lstat(const char* path, Struct* s) { return ::lstat(path, s); }
    static int fstat(int fd, Struct* s) { return ::fstat(fd, s); }

private:

    Stat();  ///< non-instantiable
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
