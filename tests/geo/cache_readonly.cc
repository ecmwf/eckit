// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Download.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/FileCache.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/geo/search/KDTreeMemory.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"

#include "cache_testing.h"


namespace eckit::geo::test {


using search::KDTree;
using search::KDTreeCache;
using search::KDTreeFactory;


const std::vector<PointXYZ> POINTS{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}};


KDTree::Fill fill() {
    return [](std::vector<KDTree::Value>& values) {
        for (size_t i = 0; i < POINTS.size(); ++i) {
            values.emplace_back(POINTS[i], i);
        }
    };
}


// ECKIT_GEO_CACHE_PATH (a provisioned cache, made read-only) is set by the test environment
CASE("read-only cache: cache files only found, otherwise computed in memory, and nothing written") {
    EXPECT(LibEcKitGeo::caching());

    const auto& paths = LibEcKitGeo::cacheRoots().paths();
    EXPECT(paths.size() == 1);

    const auto& root = paths.front();
    if (root.exists()) {
        read_only(root, false);
    }
    eckit::cache::DiskCache::remove(root);
    root.mkdir();

    const std::string uid = "test-cache-readonly";
    provision(root, uid, POINTS);

    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: O16}"));
    provision(root, Search::uid(*grid), points_xyz(*grid));

    std::unique_ptr<const Area> area(AreaFactory::make_from_string("{area: [60, 0, 30, 30]}"));
    const Grid::renumber_type provisioned{0, 1, 2};
    provision(root, *grid, *area, provisioned);

    read_only(root, true);
    const auto files = listing(root);

    EXPECT(LibEcKitGeo::cacheReadablePaths() == paths);
    EXPECT(LibEcKitGeo::cacheWritablePaths().empty());
    EXPECT_THROWS(LibEcKitGeo::cacheDir());

    SECTION("k-d tree: built in memory, unless provisioned") {
        std::unique_ptr<KDTree> missing(KDTreeFactory::build("mmap", uid + "-missing", POINTS.size()));
        EXPECT(dynamic_cast<search::KDTreeMemory*>(missing.get()) != nullptr);

        std::unique_ptr<KDTree> provisioned(KDTreeFactory::build("mmap", uid, POINTS.size()));
        EXPECT(dynamic_cast<KDTreeCache*>(provisioned.get()) != nullptr);
        EXPECT_NOT(provisioned->build(fill()));  // loaded, not built
        EXPECT(provisioned->nearest_neighbour(POINTS[1]).index == 1);
    }

    SECTION("search: of a grid, provisioned") {
        EXPECT(dynamic_cast<const KDTreeCache*>(&grid->search().kdtree()) != nullptr);
        EXPECT(grid->search().search_nn(grid->last_point()).index == grid->size() - 1);
    }

    SECTION("search: of points, in memory") {
        const Search search(std::vector<PointXYZ>{{1., 0., 0.}, {0., 1., 0.}});
        EXPECT(dynamic_cast<const search::KDTreeMemory*>(&search.kdtree()) != nullptr);
        EXPECT(search.search_nn(PointXYZ{0., 1., 0.}).index == 1);
    }

    SECTION("crop: provisioned, otherwise computed") {
        EXPECT(grid->crop(*area) == provisioned);

        std::unique_ptr<const Area> other(AreaFactory::make_from_string("{area: [50, 0, 20, 30]}"));
        EXPECT_NOT(grid->crop(*other).empty());
    }

    SECTION("coordinates: in memory only") {
        const grid::Unstructured grid(std::vector<double>{1., 2.}, std::vector<double>{3., 4.});
        EXPECT(grid.size() == 2);
        EXPECT_NOT(coordinates_cached(grid));
    }

    SECTION("downloads: not found is an error") {
        const eckit::cache::Download download(LibEcKitGeo::cacheRoots() / "download");
        EXPECT_THROWS_AS(download.toCachedPath("file:///does/not/exist/eckit_test_geo_cache_readonly"),
                         eckit::cache::CacheError);
    }

    EXPECT(listing(root) == files);

    read_only(root, false);
    eckit::cache::DiskCache::remove(root);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
