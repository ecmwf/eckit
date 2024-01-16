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


#include "eckit/geo/util/Cache.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using namespace geo::util;


CASE("Cache") {
    struct test_t {
        size_t N;
        Cache::bytes_t pl_footprint;
        Cache::bytes_t pl_footprint_acc;
    } tests[] = {
        {16, 256, 256},
        {24, 384, 640},
        {24, 384, 640},  // (repeated for a cache hit)
        {32, 512, 1152},
        {16, 256, 1152},  // (repeated for another cache hit)
        {48, 768, 1920},
        {64, 1024, 2944},
    };


    SECTION("separate caches") {
        Cache::total_purge();
        EXPECT_EQUAL(0, Cache::total_footprint());

        // add an entry to a cache
        reduced_classical_pl(16);
        auto foot = Cache::total_footprint();
        EXPECT(0 < foot);

        // add another distinct entry to a cache of same type
        reduced_octahedral_pl(16);
        EXPECT(foot < Cache::total_footprint());

        Cache::total_purge();
        EXPECT_EQUAL(0, Cache::total_footprint());
    }


    SECTION("reduced_classical_pl, reduced_octahedral_pl") {
        for (const geo::pl_type& (*cacheable)(size_t) : {&reduced_classical_pl, &reduced_octahedral_pl}) {
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

        Cache::total_purge();
        EXPECT_EQUAL(0, Cache::total_footprint());
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
