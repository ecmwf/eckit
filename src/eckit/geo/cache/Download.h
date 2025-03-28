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

#include "eckit/geo/cache/CacheRoot.h"
#include "eckit/log/Bytes.h"


namespace eckit::geo::cache {


class Download final : public CacheRoot {
public:

    // -- Types

    using url_type = std::string;

    struct info_type {
        Bytes bytes;
        double time_s;
    };

    // -- Constructors

    explicit Download(const PathName& root = ".", bool html = false) : CacheRoot(root), html_(html) {}

    // -- Methods

    PathName to_cached_path(const url_type&, const std::string& prefix = "",
                            const std::string& suffix = ".download") const;

    // -- Class methods

    static info_type to_path(const url_type&, const PathName&, bool html = false);

    static std::string url_file_basename(const url_type&, bool ext = true);
    static std::string url_file_extension(const url_type&);

private:

    void rmdir(const PathName&) const;

    bool html_;
};


}  // namespace eckit::geo::cache
