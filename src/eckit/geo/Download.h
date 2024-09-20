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

    explicit Download(const PathName& root = ".", const std::string& prefix = "",
                      const std::string& extension = ".download");

    static info_type to_path(const url_type&, const PathName&);

    PathName to_cached_path(const url_type&) const;
    void rm_cache_root() const { rmdir(root_); }

private:
    void rmdir(const PathName&) const;

    const PathName root_;
    const std::string prefix_;
    const std::string extension_;
};


}  // namespace eckit::geo
