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


#include "eckit/geo/cache/DiskCache.h"

#include "eckit/geo/util/mutex.h"


namespace eckit::geo::cache {


void DiskCache::rmdir(const PathName& p) const {
    // control concurrent access
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};

    if (!p.exists()) {
        return;
    }

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    for (auto& f : files) {
        f.unlink();
    }

    for (auto& d : dirs) {
        rmdir(d);
    }

    p.rmdir();
}


}  // namespace eckit::geo::cache
