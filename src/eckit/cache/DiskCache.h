// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <functional>
#include <optional>

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

    // -- Constants

    /// Permissions of cache files: read-only for all users, as they are replaced, never modified (processes may map
    /// them)
    static constexpr int PERMISSIONS = 0444;

    // -- Types

    /// Writes a file or directory at the given temporary path, renamed into place when complete
    using Write = std::function<void(const PathName&)>;

    /// Checks if an existing file is valid; an invalid file is recreated (getOrCreate) or not found (find)
    using Valid = Roots::Valid;

    // -- Constructors

    explicit DiskCache(const PathName& root) : DiskCache(Roots{root}) {}
    explicit DiskCache(const Roots&);

    // -- Methods

    const Roots& cacheRoots() const { return roots_; }

    std::optional<PathName> find(const PathName& relative, const Valid& = {}) const;

    /// Find a valid file, or create it in the first writable root (throws if none)
    /// @return its path
    PathName getOrCreate(const PathName& relative, const Write&, const Valid& = {}) const;

    // -- Class methods

    /// Create a file, unless it exists and is valid
    /// @return if created
    static bool create(const PathName&, const Write&, const Valid& = {});

    /// Remove a file or directory (recursively), if it exists
    static void remove(const PathName&);

    /// Size [byte] of a file, or of the files under a directory (recursively); 0 if it does not exist
    static size_t size(const PathName&);

private:

    // -- Members

    const Roots roots_;

    // -- Class methods

    /// Make files read-only for all users, as processes may map them (replaced, never modified), directories writable
    static void share(const PathName&);

    static void replace(const PathName& tmp, const PathName&);
};


}  // namespace eckit::cache
