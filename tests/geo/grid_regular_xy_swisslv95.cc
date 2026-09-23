// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <memory>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("swisslv95") {
    area::BoundingBox bounds(47.81, 5.95, 45.81, 10.5);  // from epsg.io
    const std::vector<size_t> expected_shape{294, 429};

    std::unique_ptr<const Grid> grid(
        GridFactory::build(spec::Custom{                                                        //
                                        {"type", "swisslv95"},                                  //
                                        {"x", std::vector<double>{2439000., 2867000., 1000.}},  //
                                        {"y", std::vector<double>{1040500., 1333500., 1000.}}}));

    EXPECT(grid->boundingBox().contains(bounds));
    EXPECT(grid->shape() == expected_shape);

    // alias (infer extents)
    std::unique_ptr<const Grid> grid_2(GridFactory::build(spec::Custom{{"grid", "swisslv95"}}));
    std::unique_ptr<const Grid> grid_3(GridFactory::make_from_string("swisslv95"));

    EXPECT(*grid_2 == *grid);
    EXPECT(*grid_3 == *grid);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
