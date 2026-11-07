// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/system/Library.h"


namespace eckit {


/**
 * eckit::cache: generic in-process caches (MemoryCache), cache files (FileCache, Download, Unzip) and their loading
 * (Loader)
 */
class LibEcKitCache final : public system::Library {
public:

    // -- Methods

    static LibEcKitCache& instance();

    /**
     * If a missing cache file is an error, not created, for provisioned caches
     * (eckit-cache-throw-on-miss;$ECKIT_CACHE_THROW_ON_MISS, default from CMake eckit_CACHE_THROW_ON_MISS)
     */
    static bool throwOnMiss();

    /**
     * Maximum wait for another process loading into shared memory [s], then an error; 0 waits without a limit
     * (eckit-cache-shared-memory-wait;$ECKIT_CACHE_SHARED_MEMORY_WAIT, default from CMake
     * eckit_CACHE_SHARED_MEMORY_WAIT)
     */
    static int sharedMemoryWait_s();

    /**
     * Time cache files are kept since last used [s], when limiting them to a capacity (see FileCache), as a process may
     * be about to open them (eckit-cache-file-kept;$ECKIT_CACHE_FILE_KEPT, default from CMake eckit_CACHE_FILE_KEPT)
     */
    static int fileCacheKept_s();

private:

    // -- Constructors

    LibEcKitCache();

    // -- Overridden methods

    [[nodiscard]] const void* addr() const override;
    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;
};


}  // namespace eckit
