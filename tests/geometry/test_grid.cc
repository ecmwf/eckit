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

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/Grid.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using namespace geometry;


CASE("GridFactory::build") {
    SECTION("GridFactory::build_from_name") {
        std::unique_ptr<const Grid> grid(GridFactoryName::build("O2"));

        auto size = grid->size();
        EXPECT_EQUAL(size, 88);
    }


    SECTION("Grid::build_from_uid") {
    }


    SECTION("Grid::build_from_increments") {
    }


    SECTION("Grid::build") {
        std::unique_ptr<const Grid> grid(GridFactory::build(MappedConfiguration{
            {{"name", "O2"}}}));

        auto size = grid->size();
        EXPECT_EQUAL(size, 88);
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
