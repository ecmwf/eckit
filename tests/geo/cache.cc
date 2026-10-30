// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <unistd.h>

#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "eckit/cache/MemoryUsage.h"
#include "eckit/cache/loader/File.h"
#include "eckit/cache/loader/MappedFile.h"
#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


/// eckit::geo in-memory caches, as a whole (see LibEcKitGeo)
struct Cache {
    using bytes_size_t = size_t;
    static bytes_size_t total_footprint() { return LibEcKitGeo::cacheFootprint().total(); }
    static void total_purge() { LibEcKitGeo::purgeCache(); }
};


CASE("eckit::geo::util") {
    struct test_t {
        size_t N;
        bool increasing;
        Cache::bytes_size_t pl_footprint;
        Cache::bytes_size_t pl_footprint_acc;
        Cache::bytes_size_t gl_footprint;
        Cache::bytes_size_t gl_footprint_acc;
    } tests[] = {
        {16, false, 256, 256, 256, 256},    //
        {24, false, 384, 640, 384, 640},    //
        {24, false, 384, 640, 384, 640},    // (repeated for a cache hit)
        {32, false, 512, 1152, 512, 1152},  //
        {16, false, 256, 1152, 256, 1152},  // (repeated for another cache hit)
        {48, false, 768, 1920, 768, 1920},  //
        {16, true, 256, 1920, 256, 2176},   // (repeated except for 'increasing')
        {24, true, 384, 1920, 384, 2560},   // ...
        {24, true, 384, 1920, 384, 2560},   //
        {32, true, 512, 1920, 512, 3072},   //
        {16, true, 256, 1920, 256, 3072},   //
        {48, true, 768, 1920, 768, 3840},   //
    };


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("separate caches") {
        util::reduced_classical_pl(16);
        auto foot = Cache::total_footprint();
        EXPECT(0 < foot);

        util::reduced_octahedral_pl(16);
        EXPECT(foot < Cache::total_footprint());
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("reduced_classical_pl, reduced_octahedral_pl") {
        for (const auto& fun : {util::reduced_classical_pl, util::reduced_octahedral_pl}) {
            for (const auto& test : tests) {
                Cache::total_purge();
                fun(test.N);
                EXPECT(Cache::total_footprint() == test.pl_footprint);
            }

            Cache::total_purge();
            for (const auto& test : tests) {
                fun(test.N);
                EXPECT(Cache::total_footprint() == test.pl_footprint_acc);
            }
        }
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("gaussian_latitudes") {
        for (const auto& test : tests) {
            Cache::total_purge();
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT(Cache::total_footprint() == test.gl_footprint);
        }

        Cache::total_purge();
        for (const auto& test : tests) {
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT(Cache::total_footprint() == test.gl_footprint_acc);
        }
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());
}


#if eckit_HAVE_CURL
CASE("grid") {
    const auto footprint_1 = Cache::total_footprint();

    spec::Custom spec({{"uid", "d5bde4f52ff3a9bea5629cd9ac514410"}});  // ORCA2_T
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec));

    // lazy behaviour is expected, force load only on calculate_uid
    Log::info() << "uid: '" << grid1->uid() << "'" << std::endl;

    const auto footprint_2 = Cache::total_footprint();
    EXPECT(footprint_1 == footprint_2);

#if eckit_HAVE_LZ4
    // calculate_uid requires the coordinates, which need uncompressing
    EXPECT(grid1->calculate_uid() == spec.get_string("uid"));
#endif

    const auto footprint_3 = Cache::total_footprint();
    EXPECT(footprint_2 <= footprint_3);

    std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("{uid:" + grid1->uid() + "}"));

    EXPECT(footprint_3 == Cache::total_footprint());
    EXPECT(grid1->size() == grid2->size());

    Cache::total_purge();
    EXPECT(Cache::total_footprint() <= footprint_1);
}
#endif


CASE("cache layout") {
    const PathName root{LibEcKitGeo::cacheDir()};

    // coordinates written locally (e.g. unstructured grids) go to latlon/VERSION/<uid>.ek
    const auto path = cache::LatitudeLongitude({1., 2.}, {3., 4.}).to_cached_path();
    EXPECT(path.exists());
    EXPECT(path.dirName() == root / "latlon" / std::to_string(cache::LatitudeLongitude::VERSION));
}


CASE("SearchCache: a grid search, accounted for") {
    auto& cache = geo::cache::SearchCache::instance();
    cache.clear();

    const auto capacity = cache.capacity();

    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));

    // a grid holds its search (default configuration), loading it increases the cache usage
    const auto& search = grid->search();
    EXPECT(search.footprint());
    EXPECT(cache.usage() == search.footprint());
    EXPECT(&search == &grid->search());

    // default loader (mapped-file), as mir: the cache file is mapped (page cache)
    if (LibEcKitGeo::caching()) {
        const auto* cached = dynamic_cast<const search::KDTreeCache*>(&search.kdtree());
        EXPECT(cached != nullptr);
        EXPECT(dynamic_cast<const eckit::cache::loader::MappedFile*>(&cached->loader()) != nullptr);
        EXPECT(search.footprint().shared() > 0);
    }

    // searches are cached by grid and k-d tree configuration
    const spec::Custom unbalanced{{{"search-fast-build", false}}};
    auto other = cache.get(*grid, unbalanced);
    EXPECT(other.get() != &search);
    EXPECT(cache.usage() == search.footprint() + other->footprint());
    EXPECT(cache.get(*grid, unbalanced) == other);

    // other behaviours share the k-d tree, so they are not accounted for again
    auto knn = cache.get(*grid, spec::Custom{{{"search-fast-build", false}, {"search", "knn"}, {"search-k", 4}}});
    EXPECT(&knn->kdtree() == &other->kdtree());
    EXPECT(knn->search(PointLonLat{14., 1.}).size() == 4);
    EXPECT(cache.size() == 2);

    // searches in use are not evicted (the grid's own, and the other shared by knn)
    other.reset();
    cache.capacity(eckit::cache::MemoryUsage{0, 0});
    EXPECT(cache.size() == 2);

    knn.reset();
    cache.capacity(eckit::cache::MemoryUsage{0, 0});
    EXPECT(cache.size() == 1);
    EXPECT(cache.usage() == search.footprint());

    cache.capacity(capacity);
    cache.clear();
}


CASE("SearchCache: room is made for a search before loading it") {
    auto& cache = geo::cache::SearchCache::instance();
    cache.clear();

    const auto capacity = cache.capacity();

    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom{{{"grid", "20/20"}}}));

    // as estimated (of the library's loader)
    const auto loader = LibEcKitGeo::searchCacheLoader();

    auto search1 = cache.get(*grid1);
    EXPECT(search1->footprint() == search::KDTreeFactory::estimate(loader, grid1->size()));
    EXPECT(cache.usage() == search1->footprint());

    // room for one search only
    const auto estimate2 = search::KDTreeFactory::estimate(loader, grid2->size());
    cache.capacity(estimate2);

    std::weak_ptr<const Search> evicted = search1;
    search1.reset();

    const auto evictions = cache.statistics().evictions;
    auto search2         = cache.get(*grid2);
    EXPECT(evicted.expired());
    EXPECT(cache.statistics().evictions == evictions + 1);
    EXPECT(cache.usage() == estimate2);

    // cached, so nothing is evicted
    EXPECT(cache.get(*grid2) == search2);
    EXPECT(cache.statistics().evictions == evictions + 1);

    search2.reset();
    cache.capacity(capacity);
    cache.clear();
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
