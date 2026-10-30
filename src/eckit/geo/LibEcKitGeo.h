// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <vector>

#include "eckit/cache/MemoryUsage.h"
#include "eckit/cache/Roots.h"
#include "eckit/system/Library.h"


namespace eckit {
class PathName;
}

namespace eckit::cache {
class MemoryCaches;
}


namespace eckit {


class LibEcKitGeo final : public system::Library {
public:

    static LibEcKitGeo& instance();

    static std::vector<PathName> shareArea();
    static std::vector<PathName> shareGrid();
    static std::vector<PathName> shareProjection();

    static PathName cacheArea();
    static PathName cacheGrid();
    static PathName cacheProjection();

    // -- Caching
    //
    // Expensive results (k-d trees, crops, grid coordinates) are cached in-process and in cache files, set by resources

    /**
     * If cache files are written (eckit-geo-caching;$ECKIT_GEO_CACHING, default from CMake feature GEO_CACHING);
     * otherwise the cache is read-only and misses are computed in-process, or fail (e.g. downloads)
     */
    static bool caching();

    /**
     * Cache paths (eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH, "path1:path2:...", see eckit::cache::Roots), read once:
     * searched in order, written to the first writable
     */
    static const cache::Roots& cacheRoots();

    /// Cache paths existing (searched) and writable (none if not caching), as they are when called
    static std::vector<PathName> cacheReadablePaths();
    static std::vector<PathName> cacheWritablePaths();

    /// Cache path for writing, the first writable (throws if none)
    static std::string cacheDir();

    /**
     * If a missing cache file is an error rather than computed, for provisioned caches
     * (eckit-geo-cache-throw-on-miss;$ECKIT_GEO_CACHE_THROW_ON_MISS)
     */
    static bool cacheThrowOnMiss();

    /// k-d tree loader
    static std::string searchCacheLoader();

    /// If shared memory uses huge pages (where reserved), and if its size is rounded up to the huge page size
    static bool sharedMemoryHugePages();
    static bool sharedMemoryRoundHugePages();

    /// In-process cache capacities (see eckit::cache::MemoryCache)
    static cache::MemoryUsage searchCacheCapacity();
    static cache::MemoryUsage cropCacheCapacity();

    /// In-process cache eviction policies (lru, lfu, fifo, largest; see eckit::cache::Policy)
    static std::string searchCachePolicy();
    static std::string cropCachePolicy();

    /**
     * Release in-process caches (all of eckit::geo, or by kind); searches and crops in use stay valid, but purgeCache
     * also invalidates values returned by reference (e.g. Gaussian latitudes); cache files are kept
     */
    static void purgeCache();
    static void purgeCacheSearch();
    static void purgeCacheCrop();
    static cache::MemoryUsage cacheFootprint();

    /// In-process caches of eckit::geo (see purgeCache and cacheFootprint)
    static cache::MemoryCaches& memoryCaches();

    /**
     * Caching configuration and state (JSON, sizes in bytes): paths; in-process memory (process and shared) footprint,
     * capacity, policy and statistics of searches and crops; cache files size under the cache directory
     */
    static std::string cacheInfo();

    /// Remove the cache directory for writing (see cacheDir), or parts of it
    static void removeCacheDir();
    static void removeCacheArea();
    static void removeCacheGrid();
    static void removeCacheProjection();

    static std::string url(const std::string& url_path);
    static std::string url(const std::string& url_path, const std::string& url_prefix);

    static bool projdb_is_available();
    static void projdb_set_search_paths(const std::string& db_path, const std::vector<std::string>& search_paths = {});
    static void projdb_reset();

    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;

private:

    LibEcKitGeo();

    [[nodiscard]] const void* addr() const override;
};


}  // namespace eckit
