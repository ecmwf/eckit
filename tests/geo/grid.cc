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


namespace eckit::test {


using namespace geo;


CASE("GridFactory::build") {
    struct test_t {
        std::string name;
        size_t size;
    };


    SECTION("GridFactory::build_from_name") {
        for (const auto& test : {test_t{"O2", 88}, {"f2", 32}, {"h2", 48}}) {
            std::unique_ptr<const Grid> grid(
                GridFactory::build(*std::unique_ptr<Spec>(new spec::Custom({{"grid", test.name}}))));

            auto size = grid->size();
            EXPECT_EQUAL(size, test.size);
        }
    }


    SECTION("RegularGaussian") {
        std::unique_ptr<const Grid> grid(
            GridFactory::build(*std::unique_ptr<Spec>(new spec::Custom({{"grid", "f2"}, {"south", 0}}))));

        auto nh = grid->size();
        EXPECT_EQUAL(nh, 32 / 2);
    }


    SECTION("Grid::build_from_uid") {
        // TODO
    }


    SECTION("Grid::build_from_increments") {
        SECTION("global") {
            std::unique_ptr<const Grid> global(GridFactory::build(*std::unique_ptr<Spec>(new spec::Custom({
                {"type", "regular_ll"},
                {"west_east_increment", 1},
                {"south_north_increment", 1},
            }))));

            auto size = global->size();
            EXPECT_EQUAL(size, 360 * 181);
        }


        SECTION("non-global") {
            std::unique_ptr<const Grid> grid(GridFactory::build(*std::unique_ptr<Spec>(new spec::Custom({
                {"type", "regular_ll"},
                {"west_east_increment", 1},
                {"south_north_increment", 1},
                {"north", 10},
                {"west", 1},
                {"south", 1},
                {"east", 10},
            }))));

            auto size = grid->size();
            EXPECT_EQUAL(size, 100);
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
