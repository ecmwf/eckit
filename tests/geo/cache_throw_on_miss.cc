// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <fstream>
#include <memory>
#include <vector>

#include "eckit/cache/Download.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/testing/Test.h"

#include "cache_testing.h"


namespace eckit::geo::test {


// ECKIT_GEO_CACHE_PATH (writable) and ECKIT_GEO_CACHE_THROW_ON_MISS=1 are set by the test environment
CASE("throwing on cache miss: provisioned cache files found, a miss is an error") {
    EXPECT(LibEcKitGeo::caching());
    EXPECT(LibEcKitGeo::cacheThrowOnMiss());

    const auto& paths = LibEcKitGeo::cacheRoots().paths();
    EXPECT(paths.size() == 1);

    const auto& root = paths.front();
    eckit::cache::DiskCache::remove(root);
    root.mkdir();
    EXPECT_NOT(LibEcKitGeo::cacheWritablePaths().empty());

    std::unique_ptr<const Grid> o16(GridFactory::make_from_string("{grid: O16}"));
    provision(root, Search::uid(*o16), points_xyz(*o16));

    std::unique_ptr<const Grid> o8(GridFactory::make_from_string("{grid: O8}"));

    std::unique_ptr<const Area> area(AreaFactory::make_from_string("{area: [60, 0, 30, 30]}"));
    const Grid::renumber_type provisioned{0, 1, 2};
    provision(root, *o16, *area, provisioned);

    SECTION("search: provisioned, or an error") {
        EXPECT(dynamic_cast<const search::KDTreeCache*>(&o16->search().kdtree()) != nullptr);
        EXPECT_THROWS_AS(o8->search(), eckit::cache::CacheError);
    }

    SECTION("crop: provisioned, or an error") {
        EXPECT(o16->crop(*area) == provisioned);
        EXPECT_THROWS_AS(o8->crop(*area), eckit::cache::CacheError);
    }

    SECTION("downloads: an error, not downloaded") {
        const PathName file = root / "file";
        std::ofstream(file.asString()) << "file";

        const eckit::cache::Download download(LibEcKitGeo::cacheRoots("download"));
        EXPECT_THROWS_AS(download.toCachedPath("file://" + file.asString()), eckit::cache::CacheError);
    }

    SECTION("coordinates: written from memory (not a miss)") {
        const grid::Unstructured unstructured(std::vector<double>{1., 2.}, std::vector<double>{3., 4.});
        EXPECT(coordinates_cached(unstructured));
    }

    eckit::cache::DiskCache::remove(root);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
