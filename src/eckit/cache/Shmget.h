// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <sys/shm.h>  // key_t; shmid_ds is a macro on some systems, it can't be forward declared

#include <cstddef>
#include <ctime>


namespace eckit {
class PathName;
}


namespace eckit::cache {


/**
 * System V shared memory with host fallbacks (huge pages not reserved, page sizes), accounting for segments attached
 * by this process (as eckit::Shmget)
 */
class Shmget {
public:

    // -- Types

    /// Segments attached by this process
    struct Info {
        size_t count = 0;
        size_t size  = 0;
    };

    // -- Class methods

    /**
     * Create a segment (0644), failing with EEXIST if it exists; on huge pages if requested and reserved, on 64K pages
     * where supported
     */
    static int create(key_t, size_t size, bool huge_pages = false);

    /// Get a segment, on regular pages if huge pages (SHM_HUGETLB) aren't reserved
    static int shmget(key_t, size_t size, int shmflg);

    /// Attach a segment, accounted for until detached
    static void* shmat(int shmid, const void* shmaddr, int shmflg);

    /// Size of a segment attached by shmat, 0 if not attached
    static size_t size(const void* shmaddr);

    /// Detach a segment
    static int shmdt(const void* shmaddr);

    /// Control a segment
    static int shmctl(int shmid, int cmd, shmid_ds* buf);

    /// Get the maximum segment size, 0 if unknown
    static size_t max_size();

    /// Get the huge page size, 0 if unknown or not supported
    static size_t huge_page_size();

    /// Get the segments attached by this process (count and size)
    static Info info();
};


/**
 * System V shared memory key of a file, by ftok with its modification time; not unique, as a previous version or
 * another file can share it (see loader::SharedMemory)
 */
key_t calculate_shmget_key_from_path(const PathName&);

/// As calculate_shmget_key_from_path, given the file modification time
key_t calculate_shmget_key_from_path(const PathName&, time_t modified);


}  // namespace eckit::cache
