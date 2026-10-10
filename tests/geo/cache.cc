// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/InMemoryCache.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/cache/MemoryUsage.h"
#include "eckit/geo/util.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/StringTools.h"


namespace eckit::geo::test {


const std::string URL             = "https://www.ecmwf.int/robots.txt";
const std::string URL_NOT_FOUND_1 = "https://does.not/exist";
const std::string URL_NOT_FOUND_2 = "https://sites.ecmwf.int/repository/does/not/exist";
const std::string URL_BAD_SSL     = "https://expired.badssl.com/robots.txt";


CASE("eckit::geo::util") {
    using Cache = cache::MemoryCache;

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
    EXPECT_EQUAL(0, Cache::total_footprint());


    SECTION("separate caches") {
        util::reduced_classical_pl(16);
        auto foot = Cache::total_footprint();
        EXPECT(0 < foot);

        util::reduced_octahedral_pl(16);
        EXPECT(foot < Cache::total_footprint());
    }


    Cache::total_purge();
    EXPECT_EQUAL(0, Cache::total_footprint());


    SECTION("reduced_classical_pl, reduced_octahedral_pl") {
        for (const auto& fun : {util::reduced_classical_pl, util::reduced_octahedral_pl}) {
            for (const auto& test : tests) {
                Cache::total_purge();
                fun(test.N);
                EXPECT_EQUAL(Cache::total_footprint(), test.pl_footprint);
            }

            Cache::total_purge();
            for (const auto& test : tests) {
                fun(test.N);
                EXPECT_EQUAL(Cache::total_footprint(), test.pl_footprint_acc);
            }
        }
    }


    Cache::total_purge();
    EXPECT_EQUAL(0, Cache::total_footprint());


    SECTION("gaussian_latitudes") {
        for (const auto& test : tests) {
            Cache::total_purge();
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT_EQUAL(Cache::total_footprint(), test.gl_footprint);
        }

        Cache::total_purge();
        for (const auto& test : tests) {
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT_EQUAL(Cache::total_footprint(), test.gl_footprint_acc);
        }
    }


    Cache::total_purge();
    EXPECT_EQUAL(0, Cache::total_footprint());
}


#if eckit_HAVE_CURL
CASE("download: error handling") {
    const PathName path("test.download");
    if (path.exists()) {
        path.unlink();
        ASSERT(!path.exists());
    }

    SECTION("not found") {
        EXPECT_THROWS_AS(cache::Download::to_path(URL_NOT_FOUND_1, path), UserError);
        EXPECT(!path.exists());

        EXPECT_THROWS_AS(cache::Download::to_path(URL_NOT_FOUND_2, path), UserError);
        EXPECT(!path.exists());
    }

    SECTION("bad ssl") {
        EXPECT_THROWS_AS(cache::Download::to_path(URL_BAD_SSL, path), UserError);
        EXPECT(!path.exists());
    }
}


CASE("download: non-cached") {
    const PathName path("test.download");
    if (path.exists()) {
        path.unlink();
        ASSERT(!path.exists());
    }

    auto info = cache::Download::to_path(URL, path);

    EXPECT(info.bytes.value() > 0.);
    EXPECT(path.exists());

    path.unlink();
    ASSERT(!path.exists());
}


CASE("download: cached") {
    const std::string prefix = "prefix-";
    const std::string suffix = ".suffix";

    const PathName root("test.download.dir", true);

    cache::Download download(root);
    EXPECT(root == download.cache_root());

    download.rm_cache_root();
    EXPECT(!root.exists());

    auto path = download.to_cached_path(URL, prefix, suffix);

    EXPECT(root.exists() && root.isDir());
    EXPECT(path.exists());

    std::string basename = path.baseName();
    EXPECT(StringTools::startsWith(basename, prefix));
    EXPECT(StringTools::endsWith(basename, suffix));
    EXPECT(path.dirName() == root);

    download.rm_cache_root();
    EXPECT(!root.exists());
}


CASE("grid") {
    using Cache = cache::MemoryCache;

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


CASE("MemoryUsage") {
    using cache::MemoryUsage;

    // as capacities are given: "memory[,shared]" (bytes), shared defaults to memory
    MemoryUsage a{"100,200"};
    MemoryUsage b{"300"};

    EXPECT_EQUAL(a.memory(), 100);
    EXPECT_EQUAL(a.shared(), 200);
    EXPECT_EQUAL(b.shared(), 300);
    EXPECT_EQUAL(MemoryUsage{a.str()}, a);

    EXPECT_EQUAL(a + b, MemoryUsage(400, 500));
    EXPECT_EQUAL(b - a, MemoryUsage(200, 100));
    EXPECT_EQUAL(a - b, MemoryUsage());  // saturates at zero
    EXPECT_NOT(MemoryUsage());

    // exceeding a capacity in either memory or shared memory
    EXPECT(MemoryUsage(301, 0).exceeds(b));
    EXPECT(MemoryUsage(0, 301).exceeds(b));
    EXPECT_NOT(b.exceeds(b));

    EXPECT_THROWS_AS(MemoryUsage{"1,2,3"}, BadValue);
}


CASE("InMemoryCache: usage, capacity and evictions") {
    using cache::InMemoryCache;
    using cache::MemoryUsage;
    using Cache = cache::MemoryCache;

    struct Values {
        explicit Values(size_t n) : values(n) {}
        MemoryUsage footprint() const { return {values.size() * sizeof(double), 0}; }
        std::vector<double> values;
    };

    // 50 values use 400 bytes, the capacity is 1000 bytes (in process memory)
    InMemoryCache<Values> cached(MemoryUsage{1000, 0});

    auto make = [](size_t n) { return [n]() { return std::make_shared<const Values>(n); }; };

    const auto total = Cache::total_footprint();

    cached.get("a", make(50));  // miss
    cached.get("b", make(50));  // miss
    cached.get("a", make(50));  // hit, "a" is now more recently used than "b"

    EXPECT_EQUAL(cached.usage(), MemoryUsage(800, 0));
    EXPECT_EQUAL(cached.statistics().hits, 1);
    EXPECT_EQUAL(cached.statistics().misses, 2);

    // reported in the memory caches total
    EXPECT_EQUAL(Cache::total_footprint(), total + 800);

    // over capacity: the least recently used is evicted
    cached.get("c", make(50));
    EXPECT_EQUAL(cached.size(), 2);
    EXPECT_EQUAL(cached.usage(), MemoryUsage(800, 0));
    EXPECT_EQUAL(cached.statistics().evictions, 1);

    // values in use are not evicted, even over capacity
    auto d = cached.get("d", make(200));
    EXPECT_EQUAL(cached.size(), 1);
    EXPECT_EQUAL(cached.usage(), MemoryUsage(1600, 0));

    d.reset();
    cached.capacity(MemoryUsage{1000, 0});
    EXPECT_EQUAL(cached.size(), 0);
    EXPECT_EQUAL(Cache::total_footprint(), total);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
