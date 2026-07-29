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

    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;

private:

    LibEcKitGeo();

    [[nodiscard]] const void* addr() const override;
};


}  // namespace eckit
