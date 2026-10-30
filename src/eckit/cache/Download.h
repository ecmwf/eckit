// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <map>
#include <string>

#include "eckit/cache/DiskCache.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Bytes.h"


namespace eckit::cache {


/// Files downloaded once by URL into cache directories; a failed or partial download is never cached
class Download final : public DiskCache {
public:

    // -- Types

    using url_type     = std::string;
    using headers_type = std::map<std::string, std::string>;  // keys in lowercase

    struct info_type {
        Bytes bytes;
        double time_s;
    };

    // -- Constructors

    explicit Download(const PathName& root, bool html = false) : DiskCache(root), html_(html) {}
    explicit Download(const Roots& roots, bool html = false) : DiskCache(roots), html_(html) {}

    // -- Methods

    static const std::string PREFIX;
    static const std::string SUFFIX;

    PathName to_cached_path(const url_type&, const std::string& prefix = PREFIX,
                            const std::string& suffix = SUFFIX) const;

    // -- Class methods

    static info_type to_path(const url_type&, const PathName&, bool html = false);

    static std::string validate_response(const url_type&, long code, const headers_type&, bool html = false);

    static std::string url_file_basename(const url_type&, bool ext = true);
    static std::string url_file_extension(const url_type&);

private:

    bool html_;
};


}  // namespace eckit::cache
