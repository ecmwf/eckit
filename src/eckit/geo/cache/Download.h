// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/cache/DiskCache.h"
#include "eckit/log/Bytes.h"


namespace eckit::geo::cache {


class Download final : public DiskCache {
public:

    // -- Types

    using url_type = std::string;

    struct info_type {
        Bytes bytes;
        double time_s;
    };

    // -- Constructors

    explicit Download(const PathName& root = ".", bool html = false) : DiskCache(root), html_(html) {}

    // -- Methods

    static const int VERSION;
    static const std::string PREFIX;
    static const std::string SUFFIX;

    PathName to_cached_path(const url_type&, const std::string& prefix = PREFIX,
                            const std::string& suffix = SUFFIX) const;

    // -- Class methods

    static info_type to_path(const url_type&, const PathName&, bool html = false);

    static int version() { return VERSION; }

    static std::string url_file_basename(const url_type&, bool ext = true);
    static std::string url_file_extension(const url_type&);

private:

    void rmdir(const PathName&) const;

    bool html_;
};


}  // namespace eckit::geo::cache
