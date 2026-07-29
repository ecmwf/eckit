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
#include <utility>
#include <vector>

#include "eckit/geo/grid/unstructured/FESOM.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


const std::string GRID_N   = "pi_N";
const Grid::uid_type UID_N = "bdc49d97a27e389fb86decd08a185c2f";  // {grid:pi_N}
const std::vector<size_t> SHAPE_N{3140};

const std::string GRID_C   = "pi_C";
const Grid::uid_type UID_C = "e548b74fa53eef5ab412c6061330f043";  // {grid:pi_C}


CASE("spec") {
    std::unique_ptr<spec::Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", UID_N}})));

    EXPECT(spec->get_string("type") == "FESOM");
    EXPECT(spec->get_string("arrangement") == "N");
    EXPECT(spec->get_string("uid") == UID_N);
    EXPECT(spec->get_unsigned_vector("shape") == SHAPE_N);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID_N + "}"));

    EXPECT(grid1->shape() == SHAPE_N);
    EXPECT(grid1->uid() == UID_N);
    EXPECT(grid1->calculate_uid() == UID_N);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID_N}})));
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", GRID_N}})));
    grid::unstructured::FESOM grid4(UID_N);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid1 == *grid3);
    EXPECT(*grid1 == grid4);
}


CASE("equals") {
    for (const auto& p : std::vector<std::pair<std::string, std::string>>{{UID_N, GRID_N}, {UID_C, GRID_C}}) {
        const auto& uid  = p.first;
        const auto& grid = p.second;

        std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + uid + "}"));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", uid}})));
        std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", grid}})));
        grid::unstructured::FESOM grid4(uid);

        EXPECT(*grid1 == *grid2);
        EXPECT(*grid2 == *grid3);
        EXPECT(*grid3 == grid4);
        EXPECT(grid4 == *grid1);

        EXPECT(grid2->uid() == grid3->calculate_uid());
        EXPECT(grid3->uid() == grid2->calculate_uid());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
