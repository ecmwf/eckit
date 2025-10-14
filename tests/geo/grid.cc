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


namespace eckit::geo::test {


CASE("GridFactory::build") {
    SECTION("GridFactory::build_from_name") {
        struct {
            std::string name;
            size_t size;
        } tests[]{{"O2", 88}, {"f2", 32}, {"h2", 48}};

        for (const auto& test : tests) {
            std::unique_ptr<const Grid> a(GridFactory::build(spec::Custom({{"grid", test.name}})));

            EXPECT_EQUAL(test.size, a->size());

            std::unique_ptr<const Grid> b(GridFactory::make_from_string(test.name));

            EXPECT_EQUAL(test.size, b->size());
        }
    }


    SECTION("Grid::build_from_increments (global)") {
        std::unique_ptr<const Grid> global(GridFactory::build(spec::Custom({
            {"type", "regular_ll"},
            {"grid", std::vector<double>{1, 1}},
        })));

        EXPECT_EQUAL(global->size(), 360 * 181);
    }


    SECTION("Grid::build_from_increments (non-global)") {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({
            {"type", "regular_ll"},
            {"grid", std::vector<double>{1, 1}},
            {"north", 10},
            {"west", 1},
            {"south", 1},
            {"east", 10},
        })));

        EXPECT_EQUAL(grid->size(), 100);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
