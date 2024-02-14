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

#include "eckit/geo/Cache.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using namespace geo;


CASE("GridFactory::build") {
    SECTION("GridFactory::build_from_name") {
        struct {
            std::string name;
            size_t size;
        } tests[]{{"O2", 88}, {"f2", 32}, {"h2", 48}};

        for (const auto& test : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", test.name}})));

            auto size = grid->size();
            EXPECT_EQUAL(size, test.size);
        }
    }


    SECTION("RegularGaussian") {
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


    SECTION("ReducedGaussian") {
        // different ways to instantiate the same grid (O2)
        for (auto spec : {spec::Custom({{"grid", "o2"}}),
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

        {
            std::unique_ptr<const Grid> grid1(new grid::reduced::ReducedGaussian(2));
            auto n1 = grid1->size();

            EXPECT_EQUAL(n1, 88);

            // std::unique_ptr<const Grid> grid2(grid1->crop(...));
            // auto n2 = grid2->size();

            // EXPECT_EQUAL(n2, n1 / 2);
        }

    }


    SECTION("Grid::build_from_uid") {
        spec::Custom spec({
            {"uid", "a832a12030c73928133553ec3a8d2a7e"},
        });

        const auto footprint = Cache::total_footprint();

        std::unique_ptr<const Grid> a(GridFactory::build(spec));

        const auto footprint_a = Cache::total_footprint();
        EXPECT(footprint < footprint_a);

        std::unique_ptr<const Grid> b(GridFactory::build(spec));

        const auto footprint_b = Cache::total_footprint();
        EXPECT_EQUAL(footprint_a, footprint_b);

        const auto size_a = a->size();
        const auto size_b = b->size();
        EXPECT_EQUAL(size_a, size_b);
    }


    SECTION("Grid::build_from_increments") {
        SECTION("global") {
            std::unique_ptr<const Grid> global(GridFactory::build(spec::Custom({
                {"type", "regular_ll"},
                {"west_east_increment", 1},
                {"south_north_increment", 1},
            })));

            auto size = global->size();
            EXPECT_EQUAL(size, 360 * 181);
        }


        SECTION("non-global") {
            std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({
                {"type", "regular_ll"},
                {"west_east_increment", 1},
                {"south_north_increment", 1},
                {"north", 10},
                {"west", 1},
                {"south", 1},
                {"east", 10},
            })));

            auto size = grid->size();
            EXPECT_EQUAL(size, 100);
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
