// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>

#include "eckit/geo/cache/DiskCache.h"


namespace eckit::geo::cache {


class Unzip final : public DiskCache {
public:

    // -- Constructors

    explicit Unzip(const PathName& root = ".") : DiskCache(root) {}

    // -- Methods

    PathName to_cached_path(const PathName& zip, const std::string& what = {}, const std::string& prefix = "",
                            const std::string& suffix = ".unzip") const;

    // -- Class methods

    static void to_path(const PathName& zip, const PathName&, const std::string& what = {});

    static std::vector<std::string> list(const PathName& zip, bool files_only = true);
};


}  // namespace eckit::geo::cache
