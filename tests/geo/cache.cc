// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <ostream>
#include <string>

#include "eckit/eckit_config.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


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


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
