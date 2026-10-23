// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <sys/shm.h>  // shmid_ds is a macro on macOS, it can't be forward declared

#include <cstddef>

#include "eckit/memory/Shmget.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::util {


/// System V shared memory, as eckit::Shmget, with eckit::geo's defaults
class Shmget : public eckit::Shmget {
public:

    // -- Class methods

    /// Calculate the key of a file, by ftok with its modification time (a rebuilt file has a different key)
    static key_t calculate_key(const PathName&);

    /// Get or create (0600) a segment, on huge pages if enabled (LibEcKitGeo::searchSharedMemoryHugePages)
    static int shmget(key_t, size_t size);

    /// Get a segment, on regular pages if huge pages (SHM_HUGETLB) aren't reserved
    static int shmget(key_t, size_t size, int shmflg);

    /// Control a segment
    static int shmctl(int shmid, int cmd, shmid_ds* buf);

    /// Get the maximum segment size, 0 if unknown
    static size_t max_size();
};


}  // namespace eckit::geo::util
