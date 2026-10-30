// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>

#include "eckit/cache/FileCache.h"
#include "eckit/cache/MemoryCache.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/cache/Crop.h"


namespace eckit::geo {
class Area;
class Grid;
}  // namespace eckit::geo


namespace eckit::geo::cache {


/**
 * @brief Crops shared in the process, by grid and area, within a capacity (as mir's area cache)
 * @details When caching, also shared across processes by cache file (see file_cache()), an eckit::FileStream (see
 * eckit::cache::loader::Stream) so a truncated or foreign file fails to read.
 */
class CropCache final : public eckit::cache::MemoryCache<Crop> {
public:

    // -- Methods

    std::shared_ptr<const Crop> get(const Grid&, const Area&);

    // -- Class members

    /// Cache file layout version (part of its path)
    static constexpr int VERSION = 2;

    /// Content kind, where shared between processes (see eckit::cache::Loader::Options)
    static constexpr int MAGIC = 0x63726f70;  // "crop"

    // -- Class methods

    static CropCache& instance();

    /// Cache files of crops (crop/VERSION/<key>.crop)
    static const eckit::cache::FileCache& file_cache();

    /// Grid points contained in the area, by index, as the grid; cropping unprojected grids by a bounding box, points
    /// are as the cropped grid (rows are from the bounding box west)
    static Crop::renumber_type calculate(const Grid&, const Area&);

private:

    // -- Constructors

    CropCache();
};


}  // namespace eckit::geo::cache
