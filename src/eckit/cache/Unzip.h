// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * Zip archives (entirely or one entry) extracted once into cache directories; partial extractions are never cached, and
 * entries with an absolute path or ".." are an error
 */
class Unzip final : public DiskCache {
public:

    // -- Constructors

    explicit Unzip(const PathName& root = ".") : DiskCache(root) {}
    explicit Unzip(const Roots& roots) : DiskCache(roots) {}

    // -- Methods

    /**
     * Extract once per archive version (path, size, mtime) all files (a directory) or an entry (a file); @return its
     * path in the cache directories
     */
    PathName to_cached_path(const PathName& zip, const std::string& what = {}, const std::string& prefix = "",
                            const std::string& suffix = ".unzip") const;

    // -- Class methods

    /// Extract all files into a directory, or an entry into a file
    static void to_path(const PathName& zip, const PathName&, const std::string& what = {});

    /// Entry names (directories end in '/')
    static std::vector<std::string> list(const PathName& zip, bool files_only = true);
};


}  // namespace eckit::cache
