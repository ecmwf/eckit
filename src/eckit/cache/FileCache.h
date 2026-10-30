// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/Loader.h"


namespace eckit::cache {


/**
 * @brief Cache files of one kind (e.g. k-d trees, crops), found in the cache directories or else created once
 * @details Files are <root>/<name>/<version>/<key><extension>; the version separates incompatible layouts. As
 * eckit::CacheManager, a miss can be an error (throw_on_miss). Their total size is not bounded (cache directories are
 * managed, e.g. provisioned or cleaned, outside the processes using them).
 */
class FileCache {
public:

    // -- Constructors

    FileCache(const Roots&, const std::string& name, int version, const std::string& extension,
              const Loader::Options& = {}, bool throw_on_miss = LibEcKitCache::throwOnMiss());

    FileCache(const FileCache&) = delete;
    FileCache(FileCache&&)      = delete;

    // -- Destructor

    ~FileCache() = default;

    // -- Operators

    FileCache& operator=(const FileCache&) = delete;
    FileCache& operator=(FileCache&&)      = delete;

    // -- Methods

    const Roots& roots() const { return disk_.cache_roots(); }

    /// Cache file of a key, relative to the cache directories
    PathName relative(const std::string& key) const;

    /// Find a valid file; @return if found
    bool find(const std::string& key, PathName& path, const DiskCache::Valid& = {}) const;

    /// Load a cache file with the named loader (see LoaderFactory) and this kind's options
    [[nodiscard]] Loader* load(const std::string& loader, const PathName&) const;

    /// Find a valid file, or else create it by write; @return its path
    PathName get_or_create(const std::string& key, const DiskCache::Write&, const DiskCache::Valid& = {},
                           bool* created = nullptr) const;

private:

    // -- Members

    const DiskCache disk_;
    const PathName prefix_;
    const std::string extension_;
    const Loader::Options loader_options_;
    const bool throw_on_miss_;
};


}  // namespace eckit::cache
