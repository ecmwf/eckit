/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Cache.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


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
        for (const geo::pl_type& (*cacheable)(size_t) : {&util::reduced_classical_pl, &util::reduced_octahedral_pl}) {
            for (const auto& test : tests) {
                Cache::total_purge();
                (*cacheable)(test.N);
                EXPECT_EQUAL(Cache::total_footprint(), test.pl_footprint);
            }

            Cache::total_purge();
            for (const auto& test : tests) {
                (*cacheable)(test.N);
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


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
