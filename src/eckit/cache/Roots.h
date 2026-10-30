// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * @brief Cache directories ("root1:root2:..."), searched in order and written to the first writable (none if writing
 * is disabled)
 * @details Paths are expanded by eckit::PathExpander (e.g. {ENVVAR?NAME}, {FILE?path}, {CWD}) and "~", as
 * eckit::CacheManager roots. Readability and writability are checked on each request, as directories can appear later.
 */
class Roots {
public:

    // -- Types

    /// If an existing file is valid, otherwise it is not found
    using Valid = std::function<bool(const PathName&)>;

    // -- Constructors

    Roots() = default;
    explicit Roots(const std::string& roots, bool writing = true);
    explicit Roots(const char* roots, bool writing = true) : Roots(std::string(roots), writing) {}
    explicit Roots(const PathName& root, bool writing = true);
    explicit Roots(std::vector<PathName> roots, bool writing = true);

    // -- Operators

    /// Subdirectory of each root
    Roots operator/(const std::string& sub) const;

    // -- Methods

    /// As listed
    const std::vector<PathName>& paths() const { return paths_; }

    /// Existing paths, searched for files
    std::vector<PathName> readable_paths() const;

    /// Writable or creatable paths (none if writing is disabled)
    std::vector<PathName> writable_paths() const;

    /// First existing, valid file relative to the roots; @return if found
    bool find(const PathName& relative, PathName& path, const Valid& = {}) const;

    // -- Class methods

    /**
     * Create a cache root and directories under it if missing, writable by all users (0777, as eckit::CacheManager);
     * missing parents of the root are created as any other directory of the process (not widened)
     */
    static void create_directory(const PathName& root, const PathName& relative = {});

    // -- Friends

    friend std::ostream& operator<<(std::ostream&, const Roots&);

private:

    // -- Members

    std::vector<PathName> paths_;
    bool writing_ = true;
};


}  // namespace eckit::cache
