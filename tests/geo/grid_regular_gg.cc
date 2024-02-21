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
#include "eckit/geo/grid/regular/RegularGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("RegularGaussian") {
    SECTION("gridspec") {
        struct {
            std::string name;
            size_t size;
        } tests[]{{"f2", 32}};

        for (const auto& test : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", test.name}})));

            auto size = grid->size();
            EXPECT_EQUAL(size, test.size);
        }
    }


    SECTION("crop") {
        spec::Custom spec({{"grid", "f2"}});
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
        auto n1 = grid1->size();

        EXPECT_EQUAL(n1, 32);

        spec.set("south", 0);
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec));
        auto n2 = grid2->size();

        EXPECT_EQUAL(n2, n1 / 2);

        // spec = spec::Custom{{{"grid", "f2"}, {"west", -180}}};
        // std::unique_ptr<const Grid> grid3(GridFactory::build(spec));
        // auto n3 = grid3->size();

        // EXPECT_EQUAL(n3, n1);

        // spec.set("east", 0);
        // std::unique_ptr<const Grid> grid4(GridFactory::build(spec));
        // auto n4 = grid4->size();

        // EXPECT_EQUAL(n4, 5 * 4);  // Ni * Nj

        // spec.set("east", -1.);
        // std::unique_ptr<const Grid> grid5(GridFactory::build(spec));
        // auto n5 = grid5->size();

        // EXPECT_EQUAL(n5, 4 * 4);  // Ni * Nj
    }


    SECTION("equals") {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f3"}})));
        std::unique_ptr<const Grid> grid2(new grid::regular::RegularGaussian(3));

        EXPECT(*grid1 == *grid2);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
