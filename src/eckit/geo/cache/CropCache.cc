// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/CropCache.h"

#include <algorithm>
#include <type_traits>
#include <variant>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::cache {


static_assert(std::is_same_v<Crop::renumber_type, Grid::renumber_type>);


CropCache& CropCache::instance() {
    static CropCache cache;
    return cache;
}


const eckit::cache::FileCache& CropCache::file_cache() {
    static const eckit::cache::FileCache cache(LibEcKitGeo::cacheRoots(), "crop", VERSION, ".crop", [] {
        eckit::cache::FileCache::Options options;
        options.throw_on_miss           = LibEcKitGeo::cacheThrowOnMiss();
        options.loader.magic            = MAGIC;
        options.loader.huge_pages       = LibEcKitGeo::sharedMemoryHugePages();
        options.loader.round_huge_pages = LibEcKitGeo::sharedMemoryRoundHugePages();
        return options;
    }());
    return cache;
}


CropCache::CropCache() :
    MemoryCache(LibEcKitGeo::memoryCaches(), LibEcKitGeo::cropCacheCapacity(), LibEcKitGeo::cropCachePolicy()) {}


std::shared_ptr<const Crop> CropCache::get(const Grid& grid, const Area& area) {
    MD5 md5;
    md5.add(grid.uid());
    md5.add(area.spec_str());

    const auto key = md5.digest();

    // at most every point (estimate)
    const eckit::cache::MemoryUsage estimate{grid.size() * sizeof(Crop::renumber_type::value_type), 0};

    return MemoryCache::get(key, estimate, [&]() {
        auto crop = std::make_shared<Crop>();

        // not caching: computed in memory
        if (!LibEcKitGeo::caching()) {
            crop->indices = calculate(grid, area);
            return crop;
        }

        // a cache file is valid if it reads (a corrupt one is recreated), and read once
        bool read  = false;
        auto valid = [&crop, &read](const PathName& p) {
            try {
                crop->read(p);
                return read = true;
            }
            catch (const Exception&) {
                return false;
            }
        };

        PathName path;
        if (file_cache().find(key, path, valid)) {
            return crop;
        }

        // nowhere to write: compute, unless a missing cache file is an error
        if (LibEcKitGeo::cacheWritablePaths().empty() && !LibEcKitGeo::cacheThrowOnMiss()) {
            crop->indices = calculate(grid, area);
            return crop;
        }

        path = file_cache().get_or_create(
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
    // rows (of constant latitude) are ordered from the bounding box west, as the cropped grid
    const auto* bbox = dynamic_cast<const area::BoundingBox*>(&area);
    const bool rows  = bbox != nullptr && grid.projection().is_default();

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
            std::stable_sort(row, end, [](const auto& a, const auto& b) { return a.lon < b.lon; });
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
