// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
