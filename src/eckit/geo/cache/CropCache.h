// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/cache/Crop.h"
#include "eckit/geo/cache/InMemoryCache.h"


namespace eckit::geo {
class Area;
class Grid;
}  // namespace eckit::geo


namespace eckit::geo::cache {


/**
 * @brief Crops shared in the process, by grid and area, within a capacity (as mir's area cache)
 * @details When caching, crops are loaded from the cache directory (crop/VERSION/<key>.crop), or calculated and saved
 * there exclusively across processes (eckit::CacheManager: the creation is locked, written to a process-specific file
 * then renamed, so other processes never read a partial file). Capacity is from resource
 * eckit-geo-cache-crop-capacity;$ECKIT_GEO_CACHE_CROP_CAPACITY, as "memory[,shared]" (bytes).
 */
class CropCache final : public InMemoryCache<Crop> {
public:

    // -- Methods

    std::shared_ptr<const Crop> get(const Grid&, const Area&);

    // -- Class methods

    static CropCache& instance();

    /// Grid points contained in the area, by index, as the grid; cropping unprojected grids by a bounding box, points
    /// are as the cropped grid (rows are from the bounding box west)
    static Crop::renumber_type calculate(const Grid&, const Area&);

private:

    // -- Constructors

    CropCache();
};


}  // namespace eckit::geo::cache
