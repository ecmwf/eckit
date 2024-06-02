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


#include <memory>

#include "eckit/geo/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


#define EXPECT_EQUAL_VECTOR(x, y)               \
    {                                           \
        EXPECT_EQUAL(x.size(), y.size());       \
        for (size_t i = 0; i < x.size(); ++i) { \
            EXPECT_EQUAL(x[i], y[i]);           \
        }                                       \
    }


namespace eckit::geo::test {


CASE("HEALPix") {
    SECTION("HEALPix::reorder") {
        std::unique_ptr<Spec> spec(new spec::Custom({{"grid", "H2"}}));
        std::unique_ptr<const Grid> ring(GridFactory::build(*spec));

        static const Renumber expected_ren_ring_to_nested{
            3,  7,  11, 15, 2,  1,  6,  5,  10, 9,  14, 13, 19, 0,  23, 4,  27, 8,  31, 12, 17, 22, 21, 26,
            25, 30, 29, 18, 16, 35, 20, 39, 24, 43, 28, 47, 34, 33, 38, 37, 42, 41, 46, 45, 32, 36, 40, 44,
        };

        const Renumber expected_ren_nested_to_ring{
            13, 5,  4,  0,  15, 7,  6,  1,  17, 9,  8,  2,  19, 11, 10, 3,  28, 20, 27, 12, 30, 22, 21, 14,
            32, 24, 23, 16, 34, 26, 25, 18, 44, 37, 36, 29, 45, 39, 38, 31, 46, 41, 40, 33, 47, 43, 42, 35,
        };

        const Renumber expected_ren_none{
            0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
            24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        };

        auto order_ring = ring->ordering();
        EXPECT_EQUAL(order_ring, Ordering::healpix_ring);

        auto ren_to_ring = ring->reorder(Ordering::healpix_ring);
        EXPECT_EQUAL_VECTOR(ren_to_ring, expected_ren_none);

        auto ren_to_nested = ring->reorder(Ordering::healpix_nested);
        EXPECT_EQUAL_VECTOR(ren_to_nested, expected_ren_ring_to_nested);


        std::unique_ptr<const Grid> nested(ring->make_grid_reordered(Ordering::healpix_nested));

        auto order_nested = nested->ordering();
        EXPECT_EQUAL(order_nested, Ordering::healpix_nested);

        ren_to_nested = nested->reorder(Ordering::healpix_nested);
        EXPECT_EQUAL_VECTOR(ren_to_nested, expected_ren_none);

        ren_to_ring = nested->reorder(Ordering::healpix_ring);
        EXPECT_EQUAL_VECTOR(ren_to_ring, expected_ren_nested_to_ring);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
