// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/cache/MemoryUsage.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::cache {


/// Grid points within an area, by index (as mir's AreaCacheEntry)
struct Crop {
    // -- Types

    using renumber_type = std::vector<size_t>;

    // -- Members

    renumber_type indices;

    // -- Methods

    eckit::cache::MemoryUsage footprint() const;

    void read(const PathName&);
    void write(const PathName&) const;
};


}  // namespace eckit::geo::cache
