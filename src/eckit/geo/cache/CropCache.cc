// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/CropCache.h"

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "eckit/container/CacheManager.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::cache {


static_assert(std::is_same_v<Crop::renumber_type, Grid::renumber_type>);


namespace {


/// Crops in the cache directory (as mir's AreaCropperCache)
struct CropCacheTraits {
    using value_type = Crop;
    using Locker     = CacheManagerFileFlock;

    static const char* name() { return "crop"; }
    static int version() { return version::CROP; }
    static const char* extension() { return ".crop"; }

    static void save(const CacheManagerBase&, const value_type& crop, const PathName& path) { crop.write(path); }
    static void load(const CacheManagerBase&, value_type& crop, const PathName& path) { crop.read(path); }
};


using CropDiskCache = CacheManager<CropCacheTraits>;


const CropDiskCache& disk_cache() {
    static const CropDiskCache CACHE(CropCacheTraits::name(), LibEcKitGeo::cacheDir(), false, 0);
    return CACHE;
}


}  // namespace


CropCache& CropCache::instance() {
    static CropCache cache;
    return cache;
}


CropCache::CropCache() : InMemoryCache(MemoryUsage{LibEcKitGeo::cropCacheCapacity()}) {}


std::shared_ptr<const Crop> CropCache::get(const Grid& grid, const Area& area) {
    MD5 md5;
    md5.add(grid.uid());
    md5.add(area.spec_str());
    const auto key = md5.digest();

    // at most every point (estimate)
    const MemoryUsage estimate{grid.size() * sizeof(Crop::renumber_type::value_type), 0};

    // requests of the same key are serialised (InMemoryCache), so the disk cache file lock (per process) is safe
    return InMemoryCache::get(key, estimate, [&]() {
        auto crop = std::make_shared<Crop>();

        if (!LibEcKitGeo::caching()) {
            crop->indices = calculate(grid, area);
            return crop;
        }

        struct Creator final : CropDiskCache::CacheContentCreator {
            Creator(const Grid& grid, const Area& area) : grid_(grid), area_(area) {}
            void create(const PathName&, Crop& crop, bool& /*saved*/) override {
                crop.indices = calculate(grid_, area_);
            }
            const Grid& grid_;
            const Area& area_;
        } creator(grid, area);

        disk_cache().getOrCreate(key, creator, *crop);
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
