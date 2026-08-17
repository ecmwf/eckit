/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>
#include <vector>

#include "eckit/system/Library.h"


namespace eckit {
class PathName;
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

    static bool caching();
    static std::string cacheDir();

    static void purgeCacheDir();
    static void purgeCacheArea();
    static void purgeCacheGrid();
    static void purgeCacheProjection();

    static std::string url(const std::string& url_path);

    static bool proj();
    /// Ensure eckit's PROJ subsystem can locate a usable PROJ database, using
    /// the given database only as a fallback if one cannot already be found.
    ///
    /// Resolution order:
    ///   1. If @c PROJ_DATA or @c PROJ_LIB is set in the environment, it is
    ///      respected unconditionally and this is a no-op (even if it turns out
    ///      to point at an incompatible database -- that is the user's choice).
    ///   2. Otherwise, if PROJ can already resolve a database on its own (via
    ///      its compiled-in default search paths, e.g. a system install), it is
    ///      left untouched.
    ///   3. Only if neither of the above yields a database, and
    ///      @p fallback_db points at an existing @c proj.db file, is the PROJ
    ///      default context pointed at it, with @p fallback_search_paths used to
    ///      locate @c proj.ini and grid files.
    ///
    /// The fallback is applied via the PROJ per-context API, so it affects only
    /// eckit's libproj instance and never leaks into other PROJ users in the
    /// process (pyproj, fiona, GDAL, ...). It is a no-op when eckit was built
    /// without PROJ support. Intended to be called once, at initialisation,
    /// before any PROJ-backed projection is created.
    ///
    /// @return true if PROJ has a usable database after the call, false otherwise.
    static bool ensureProjDatabase(const std::string& fallback_db,
                                   const std::vector<std::string>& fallback_search_paths = {});

    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;

private:

    LibEcKitGeo();

    [[nodiscard]] const void* addr() const override;
};


}  // namespace eckit
