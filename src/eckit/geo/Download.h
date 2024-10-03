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

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Bytes.h"


namespace eckit::geo {


struct Download final {
    using url_type = std::string;

    struct info_type {
        Bytes bytes;
        double time_s;
    };

    explicit Download(const PathName& root = ".", bool html = false);

    static info_type to_path(const url_type&, const PathName&, bool html = false);

    PathName to_cached_path(const url_type&, const std::string& prefix = "",
                            const std::string& extension = ".download") const;

    void rm_cache_path(const url_type&);
    void rm_cache_root() const { rmdir(root_); }

private:
    PathName cached_path(const url_type&) const;

    void rmdir(const PathName&) const;

    const PathName root_;
    bool html_;
};


}  // namespace eckit::geo
