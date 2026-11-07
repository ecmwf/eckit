// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <map>
#include <string>

#include "eckit/cache/DiskCache.h"
#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * Files downloaded once by URL into cache directories; a failed or partial download is never cached. An HTML response
 * is an error unless allowed, as it is usually not data (e.g. a captive portal)
 */
class Download final : public DiskCache {
public:

    // -- Types

    using url_type     = std::string;
    using headers_type = std::map<std::string, std::string>;  // keys in lowercase

    // -- Constructors

    explicit Download(const PathName& root, bool html = false) : DiskCache(root), html_(html) {}
    explicit Download(const Roots& roots, bool html = false) : DiskCache(roots), html_(html) {}

    // -- Methods

    /// Download a URL once, to a file named by its MD5 (any URL makes a valid file name)
    /// @return its path
    PathName toCachedPath(const url_type&, const std::string& prefix = "",
                          const std::string& suffix = ".download") const;

    // -- Class methods

    /// Check a response (a 2xx status, and no unexpected HTML)
    /// @return the error, empty if valid
    static std::string validate_response(const url_type&, long code, const headers_type&, bool html = false);

    static std::string url_file_basename(const url_type&, bool ext = true);
    static std::string url_file_extension(const url_type&);

private:

    bool html_;
};


}  // namespace eckit::cache
