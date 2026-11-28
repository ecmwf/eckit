// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/CropCache.h"

#include <algorithm>
#include <exception>
#include <type_traits>
#include <variant>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::cache {


static_assert(std::is_same_v<Crop::renumber_type, Grid::renumber_type>);


CropCache& CropCache::instance() {
    static CropCache cache;
    return cache;
}


const eckit::cache::FileCache& CropCache::file_cache() {
    static const eckit::cache::FileCache cache(
        LibEcKitGeo::cacheRoots(), "crop", VERSION, ".crop",
        [] {
            eckit::cache::Loader::Options options;
            options.magic            = MAGIC;
            options.huge_pages       = LibEcKitGeo::sharedMemoryHugePages();
            options.round_huge_pages = LibEcKitGeo::sharedMemoryRoundHugePages();
            return options;
        }(),
        LibEcKitGeo::cacheThrowOnMiss());
    return cache;
}


CropCache::CropCache() :
    MemoryCache(LibEcKitGeo::memoryCaches(), LibEcKitGeo::cropCacheCapacity(), LibEcKitGeo::cropCachePolicy()) {}


std::string CropCache::key(const Grid& grid, const Area& area) {
    MD5 md5;
    md5.add(grid.uid());
    md5.add(area.spec_str());
    return md5.digest();
}


std::shared_ptr<const Crop> CropCache::get(const Grid& grid, const Area& area) {
    const auto key = CropCache::key(grid, area);

    // at most every point (estimate)
    const eckit::cache::MemoryUsage estimate{grid.size() * sizeof(Crop::renumber_type::value_type), 0};

    return MemoryCache::get(key, estimate, [&]() {
        auto crop = std::make_shared<Crop>();

        // a cache file is valid if it reads as indices of the grid (a corrupt one is recreated), and read once
        bool read  = false;
        auto valid = [&crop, &read, size = grid.size()](const PathName& p) {
            try {
                crop->read(p);
            }
            catch (const std::exception&) {
                return false;
            }
            return read = std::all_of(crop->indices.begin(), crop->indices.end(), [size](auto i) { return i < size; });
        };

        if (file_cache().find(key, valid)) {
            return crop;
        }

        // nowhere to write: compute, unless a missing cache file is an error
        if (LibEcKitGeo::cacheWritablePaths().empty() && !LibEcKitGeo::cacheThrowOnMiss()) {
            crop->indices = calculate(grid, area);
            return crop;
        }

        const auto path = file_cache().getOrCreate(
            key,
            [&](const PathName& tmp) {
                Crop created;
                created.indices = calculate(grid, area);
                created.write(tmp);
            },
            valid);

        if (!read) {
            crop->read(path);
        }
        return crop;
    });
}


Crop::renumber_type CropCache::calculate(const Grid& grid, const Area& area) {
    // rows (of constant latitude) are ordered from the bounding box west, or east for reduced grids scanning i
    // negatively, as the cropped grid
    const auto* bbox      = dynamic_cast<const area::BoundingBox*>(&area);
    const auto* reduced   = dynamic_cast<const grid::Reduced*>(&grid);
    const bool rows       = bbox != nullptr && grid.projection().is_default();
    const bool i_negative = reduced != nullptr && !reduced->scan().is_scan_i_positive();

    struct Contained {
        double lat;
        double lon;
        size_t index;
    };

    std::vector<Contained> contained;
    size_t index = 0;
    for (const auto& p : grid) {
        if (!std::holds_alternative<PointLonLat>(p)) {
            throw exception::AreaError("CropCache: grid points are not (lon, lat)", Here());
        }

        if (area.contains(p)) {
            const auto& q = std::get<PointLonLat>(p);
            contained.push_back(
                {q.lat(), rows ? PointLonLat::normalise_angle_to_minimum(q.lon(), bbox->west()) : 0., index});
        }

        ++index;
    }

    if (rows) {
        for (auto row = contained.begin(); row != contained.end();) {
            auto end = std::find_if(row, contained.end(), [lat = row->lat](const auto& c) { return c.lat != lat; });
            std::stable_sort(row, end, [i_negative](const auto& a, const auto& b) {
                return i_negative ? b.lon < a.lon : a.lon < b.lon;
            });
            row = end;
        }
    }

    Crop::renumber_type indices;
    indices.reserve(contained.size());
    for (const auto& c : contained) {
        indices.push_back(c.index);
    }

    return indices;
}


}  // namespace eckit::geo::cache
