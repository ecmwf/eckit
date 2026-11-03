// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/unstructured/FESOM.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


const std::string GRID_N   = "CORE2_N";
const Grid::uid_type UID_N = "1d03f0efefdc1a3a8798ad53ee983140";  // {grid:CORE2_N}
const std::vector<size_t> SHAPE_N{126858};

const std::string GRID_C   = "CORE2_C";
const Grid::uid_type UID_C = "9930995170a9d054187b13962a648880";  // {grid:CORE2_C}
const std::vector<size_t> SHAPE_C{244659};


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


CASE("name") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", GRID_N}})));

    EXPECT(grid->name() == "CORE2");
    EXPECT(grid->arrangement() == "N");
}


CASE("type") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", GRID_N}})));

    EXPECT(grid->type() == "FESOM");
}


CASE("cell centres") {
    // as FESOM's elem_center (the mean of the node coordinates), the first cell
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", GRID_C}})));
    EXPECT(grid->shape() == SHAPE_C);

    const auto [lat, lon] = grid->to_latlons();
    EXPECT(lat.size() == SHAPE_C.front());
    EXPECT(types::is_approximately_equal(lon.front(), -153.771, 1e-3));
    EXPECT(types::is_approximately_equal(lat.front(), -76.8972, 1e-4));
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
