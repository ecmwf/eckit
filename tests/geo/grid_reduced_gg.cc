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
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("ReducedGaussian") {
    SECTION("gridspec") {
        // different ways to instantiate the same grid (O2)
        for (auto spec : {
                 spec::Custom({{"grid", "o2"}}),
                 spec::Custom({{"N", 2}}),
                 spec::Custom({{"pl", pl_type{20, 24, 24, 20}}}),
             }) {
            std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
            auto n1 = grid1->size();

            EXPECT_EQUAL(n1, 88);

            spec.set("south", 0);
            std::unique_ptr<const Grid> grid2(GridFactory::build(spec));
            auto n2 = grid2->size();

            EXPECT_EQUAL(n2, n1 / 2);
        }
    }


    SECTION("crop") {
        std::unique_ptr<const Grid> grid1(new grid::reduced::ReducedGaussian(2));
        auto n1 = grid1->size();

        EXPECT_EQUAL(n1, 88);

        // std::unique_ptr<const Grid> grid2(grid1->crop(...));
        // auto n2 = grid2->size();

        // EXPECT_EQUAL(n2, n1 / 2);
    }


    SECTION("equals") {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "o3"}})));
        std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("N: 3"));
        std::unique_ptr<const Grid> grid3(new grid::reduced::ReducedGaussian(pl_type{20, 24, 28, 28, 24, 20}));

        EXPECT(*grid1 == *grid2);
        EXPECT(*grid2 == *grid3);
        EXPECT(*grid3 == *grid1);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
