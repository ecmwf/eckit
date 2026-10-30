// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/FileCache.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using search::KDTree;
using search::KDTreeCache;


// ECKIT_GEO_CACHE_PATH is set by the test environment, as "<read-only>:{ENVVAR?ECKIT_GEO_TEST_CACHE_RW}"
CASE("cache directories: found in order, written to the first writable") {
    const auto& paths = LibEcKitGeo::cacheRoots().paths();
    EXPECT(paths.size() == 2);

    const PathName ro(paths.front());
    const PathName rw(paths.back());

    const auto* env = ::getenv("ECKIT_GEO_TEST_CACHE_RW");
    EXPECT(env != nullptr && rw == PathName(env));

    // permissions are not enforced for the super-user
    if (::geteuid() == 0) {
        Log::warning() << "running as the super-user, skipping" << std::endl;
        return;
    }

    eckit::cache::DiskCache::remove(ro);
    eckit::cache::DiskCache::remove(rw);
    ro.mkdir();
    ::chmod(ro.localPath(), 0555);

    EXPECT(LibEcKitGeo::cacheDir() == rw.asString());
    EXPECT(LibEcKitGeo::cacheWritablePaths() == std::vector<PathName>{rw});

    const std::vector<PointXYZ> points{{1., 0., 0.}, {0., 1., 0.}, {0., 0., 1.}};
    const auto fill = [&points](std::vector<KDTree::Value>& values) {
        for (size_t i = 0; i < points.size(); ++i) {
            values.emplace_back(points[i], i);
        }
    };

    const std::string uid = "test-cache-roots-" + std::to_string(::getpid());
    const auto relative   = geo::cache::SearchCache::file_cache().relative(uid);

    SECTION("k-d tree: built in the writable root, then found in the read-only root") {
        {
            KDTreeCache kdtree("mmap", uid, points.size());
            EXPECT(kdtree.build(fill));
            EXPECT(kdtree.path() == rw / relative);
        }

        // as a read-only cache, provisioned
        ::chmod(ro.localPath(), 0755);
        (ro / relative).dirName().mkdir();
        PathName::rename(rw / relative, ro / relative);
        ::chmod(ro.localPath(), 0555);

        KDTreeCache kdtree("mmap", uid, points.size());
        EXPECT_NOT(kdtree.build(fill));
        EXPECT(kdtree.path() == ro / relative);
        EXPECT(kdtree.nearest_neighbour(points[1]).index == 1);
    }

    SECTION("coordinates: written in the writable root") {
        const auto path = cache::LatitudeLongitude({1., 2.}, {3., 4.}).to_cached_path();
        EXPECT(path.asString().rfind(rw.asString(), 0) == 0);
    }

    ::chmod(ro.localPath(), 0755);
    eckit::cache::DiskCache::remove(ro);
    eckit::cache::DiskCache::remove(rw);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
