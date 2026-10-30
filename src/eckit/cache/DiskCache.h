// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <functional>

#include "eckit/cache/Roots.h"
#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * @brief Cache files in cache directories (see Roots), created once and never seen partially written
 * @details Creation is exclusive across threads and processes (PathLock), writing a temporary next to the file and
 * renaming it into place (atomic), so a file is complete or absent. Created files are read-only: replaced when invalid,
 * never modified, as processes may map them. Files are found in root order, or created in the first writable root.
 */
class DiskCache {
public:

    // -- Types

    /// Write the file (or directory) at the given (temporary) path
    using Write = std::function<void(const PathName&)>;

    /// If an existing file is valid, otherwise it is recreated (or not found)
    using Valid = Roots::Valid;

    // -- Constructors

    explicit DiskCache(const PathName& root) : DiskCache(Roots{root}) {}
    explicit DiskCache(const Roots&);

    // -- Methods

    const Roots& cache_roots() const { return roots_; }

    /// The first root
    const PathName& cache_root() const { return roots_.paths().front(); }

    /// Remove the first root, recursively
    void rm_cache_root() const { remove(cache_root()); }

    /// Find a valid file relative to the cache directories; @return if found
    bool find(const PathName& relative, PathName& path, const Valid& = {}) const;

    /// Find a valid file, or create it by write in the first writable root (throws if none); @return its path
    PathName get_or_create(const PathName& relative, const Write&, const Valid& = {}, bool* created = nullptr) const;

    // -- Class methods

    /// Create the file by write, unless it exists and is valid; @return if created
    static bool create(const PathName&, const Write&, const Valid& = {});

    /// Remove a file or directory (recursively), if it exists
    static void remove(const PathName&);

private:

    // -- Members

    const Roots roots_;

    // -- Class methods

    /// Created files are read-only for all users (never modified, only replaced), directories writable by all users
    static void share(const PathName&);

    /// Replace a cache file (or directory) by a complete one
    static void replace(const PathName& tmp, const PathName&);
};


}  // namespace eckit::cache
