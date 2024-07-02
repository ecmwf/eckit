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
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


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


    SECTION("Grid::build_from_increments (global)") {
        std::unique_ptr<const Grid> global(GridFactory::build(spec::Custom({
            {"type", "regular_ll"},
            {"west_east_increment", 1},
            {"south_north_increment", 1},
        })));

        auto size = global->size();
        EXPECT_EQUAL(size, 360 * 181);
    }


    SECTION("Grid::build_from_increments (non-global)") {
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


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
