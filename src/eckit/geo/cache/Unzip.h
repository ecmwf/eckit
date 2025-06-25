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

#include "eckit/geo/cache/CacheRoot.h"


namespace eckit::geo::cache {


class Unzip final : public CacheRoot {
public:

    // -- Constructors

    explicit Unzip(const PathName& root = ".") : CacheRoot(root) {}

    // -- Methods

    PathName to_cached_path(const PathName& zip, const std::string& what = {}, const std::string& prefix = "",
                            const std::string& extension = ".unzip") const;

    // -- Class methods

    static void to_path(const PathName& zip, const PathName&, const std::string& what = {});

    static std::vector<std::string> list(const PathName& zip, bool files_only = true);
};


}  // namespace eckit::geo::cache
