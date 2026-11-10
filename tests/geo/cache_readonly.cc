// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Download.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/FileCache.h"
#include "eckit/container/KDTree.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/geo/search/KDTreeMemory.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


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


std::map<std::string, std::uintmax_t> listing(const PathName& root) {
    std::map<std::string, std::uintmax_t> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root.asString())) {
        files[entry.path().string()] = entry.is_regular_file() ? entry.file_size() : 0;
    }
    return files;
}


void read_only(const PathName& root, bool yes) {
    using std::filesystem::perms;
    const auto p = perms::owner_read | perms::owner_exec | perms::group_read | perms::group_exec | perms::others_read |
                   perms::others_exec | (yes ? perms::none : perms::owner_write);

    std::filesystem::permissions(root.asString(), p);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root.asString())) {
        if (entry.is_directory()) {
            std::filesystem::permissions(entry.path(), p);
        }
    }
}


// ECKIT_GEO_CACHE_PATH and ECKIT_GEO_CACHING=0 are set by the test environment
CASE("read-only cache: cache files only found, otherwise computed in memory, and nothing written") {
    EXPECT_NOT(LibEcKitGeo::caching());

    // not caching: searches in memory
    EXPECT(LibEcKitGeo::searchCacheLoader() == "memory");

    const auto& paths = LibEcKitGeo::cacheRoots().paths();
    EXPECT(paths.size() == 1);

    const auto& root = paths.front();
    if (root.exists()) {
        read_only(root, false);
    }
    eckit::cache::DiskCache::remove(root);
    root.mkdir();

    EXPECT(LibEcKitGeo::cacheReadablePaths() == paths);
    EXPECT(LibEcKitGeo::cacheWritablePaths().empty());
    EXPECT_THROWS(LibEcKitGeo::cacheDir());

    // provisioned, then read-only
    const std::string uid = "test-cache-readonly";
    {
        const auto path = root / geo::cache::SearchCache::file_cache().relative(uid);
        path.dirName().mkdir();

        std::vector<KDTreeMapped<search::Traits>::Value> values;
        for (size_t i = 0; i < POINTS.size(); ++i) {
            values.emplace_back(POINTS[i], i);
        }

        AutoUmask umask(0);
        KDTreeMapped<search::Traits>(path, POINTS.size(), 0).build(values);
    }

    read_only(root, true);
    const auto files = listing(root);

    SECTION("k-d tree: built in memory, unless provisioned") {
        std::unique_ptr<KDTree> missing(KDTreeFactory::build("mmap", uid + "-missing", POINTS.size()));
        EXPECT(dynamic_cast<search::KDTreeMemory*>(missing.get()) != nullptr);

        std::unique_ptr<KDTree> provisioned(KDTreeFactory::build("mmap", uid, POINTS.size()));
        EXPECT(dynamic_cast<KDTreeCache*>(provisioned.get()) != nullptr);
        EXPECT_NOT(provisioned->build(fill()));  // loaded, not built
        EXPECT(provisioned->nearest_neighbour(POINTS[1]).index == 1);
    }

    SECTION("search: of a grid") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: O16}"));
        EXPECT(grid->search().search_nn(grid->last_point()).index == grid->size() - 1);
    }

    SECTION("crop: computed") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: O16}"));
        std::unique_ptr<const Area> area(AreaFactory::make_from_string("{area: [60, 0, 30, 30]}"));

        EXPECT_NOT(grid->crop(*area).empty());

        std::unique_ptr<const Grid> cropped(grid->make_grid_cropped(*area));
        EXPECT(cropped->size() == grid->crop(*area).size());
    }

    SECTION("coordinates: in memory only") {
        const grid::Unstructured grid(std::vector<double>{1., 2.}, std::vector<double>{3., 4.});
        EXPECT(grid.size() == 2);
        EXPECT_NOT(grid.spec().has("cached_path"));
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
