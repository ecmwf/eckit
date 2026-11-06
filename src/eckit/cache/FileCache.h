// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/Loader.h"


namespace eckit::cache {


/**
 * @brief Cache files of one kind, found in the cache directories or else created once
 * @details Files are <root>/<name>/<version>/<key><extension>; the version separates incompatible layouts. A miss can
 * be an error (throw_on_miss), and the total size can be limited (capacity [byte], 0 without a limit): creating a file
 * removes the least recently used files of the kind, of any version, in the first writable root. There is no index:
 * this relies on the access time of the files, set when used (the modification time is the version of a file, see
 * loader::SharedMemory). Recently used files are kept (see LibEcKitCache::fileCacheKept_s), so the capacity can be
 * exceeded for a while. Processes that mapped or attached a removed file are unaffected, and provisioned (read-only)
 * cache directories aren't managed.
 */
class FileCache {
public:

    // -- Constructors

    FileCache(const Roots&, const std::string& name, int version, const std::string& extension,
              const Loader::Options& = {}, bool throw_on_miss = LibEcKitCache::throwOnMiss(), size_t capacity = 0);

    FileCache(const FileCache&) = delete;
    FileCache(FileCache&&)      = delete;

    // -- Destructor

    ~FileCache() = default;

    // -- Operators

    FileCache& operator=(const FileCache&) = delete;
    FileCache& operator=(FileCache&&)      = delete;

    // -- Methods

    PathName relative(const std::string& key) const;

    std::optional<PathName> find(const std::string& key, const DiskCache::Valid& = {}) const;

    /// Load a cache file with the named loader (see LoaderFactory) and this kind's options
    std::unique_ptr<Loader> load(const std::string& loader, const PathName&) const;

    /// Find a valid file, or else create it, unless a miss is an error (throw_on_miss)
    /// @return its path
    PathName getOrCreate(const std::string& key, const DiskCache::Write&, const DiskCache::Valid& = {}) const;

private:

    // -- Members

    const DiskCache disk_;
    const PathName name_;
    const PathName prefix_;
    const std::string extension_;
    const Loader::Options loader_options_;
    const bool throw_on_miss_;
    const size_t capacity_;

    // -- Methods

    /// Remove least recently used files, to fit the capacity; failing to is not an error, a file was created
    void limit() const;
};


}  // namespace eckit::cache
