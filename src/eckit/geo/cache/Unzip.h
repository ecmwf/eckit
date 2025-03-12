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

#include "eckit/filesystem/PathName.h"


namespace eckit::geo::cache {


class Unzip final {
public:
    // -- Constructors

    explicit Unzip(const PathName& root = ".");

    // -- Methods

    void to_path(const PathName& zip, const PathName&, const std::string& what = {}) const;
    PathName to_cached_path(const PathName& zip, const std::string& what = {}, const std::string& prefix = "",
                            const std::string& extension = ".unzip") const;

    // -- Class methods

    static std::vector<std::string> list(const PathName& zip, bool files_only = true);

private:
    // -- Members

    const PathName root_;
};


void unzip(const PathName& zip, const PathName&, const std::string& what = {});


}  // namespace eckit::geo::cache
