/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to itr by virtue of its status as an intergovernmental organisation nor
 * does itr submit to any jurisdiction.
 */


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/grid/unstructured/ICON.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


const std::string GRID   = "icon-grid-0055-r02b05-n";
const Grid::uid_type UID = "e234e01a8556e9a84bcb42361d2f24e0";
const std::vector<long> SHAPE{2656};


CASE("spec") {
    std::unique_ptr<spec::Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", UID}})));

    EXPECT(spec->get_string("type") == "ICON");
    EXPECT(spec->get_string("name") == GRID);
    EXPECT(spec->get_string("icon_number_of_grid_used") == "55");
    EXPECT(spec->get_string("icon_type") == "hrz_regional");
    EXPECT(spec->get_string("arrangement") == "C");
    EXPECT(spec->get_string("uid") == UID);
    EXPECT(spec->get_long_vector("shape") == SHAPE);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID + "}"));

    EXPECT(grid1->size() == SHAPE[0]);
    EXPECT(grid1->uid() == UID);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID}})));
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", GRID}})));
    grid::unstructured::ICON grid4(UID);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid1 == *grid3);
    EXPECT(*grid1 == grid4);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID + "}"));
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID}})));
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", GRID}})));
    grid::unstructured::ICON grid4(UID);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == grid4);
    EXPECT(grid4 == *grid1);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
