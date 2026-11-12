// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


// Lambert conformal conic grids by name (see grid.yaml), and their first, second and last points as computed by PROJ
// (independently of eckit::geo) from their definition, and their bounding boxes as of their edges
struct test_t {
    std::string name;
    std::vector<size_t> shape;  // ny, nx
    PointLonLat first;
    PointLonLat second;
    PointLonLat last;
    std::array<double, 4> bbox;  // north, west, south, east
};


const std::vector<test_t> TESTS{
    {"meps-metno",
     {969, 849},
     {1.60491, 51.633816},
     {1.639692836822, 51.638385499539},
     {49.415261550660, 71.246014471081},
     {74.130129, -13.149065, 51.119411, 49.415262}},
    {"meps-fmi",
     {1069, 949},
     {0.278281, 50.319618},
     {0.311795398958, 50.324612028147},
     {54.237754851021, 71.575746459448},
     {75.227857, -18.121000, 49.765594, 54.237755}},
};


// as the edges are sampled (microdegrees, and the sampling)
constexpr double BBOX_TOLERANCE = 1e-5;


void check_points(const Grid& grid, const test_t& test) {
    EXPECT(grid.shape() == test.shape);
    EXPECT(grid.size() == test.shape[0] * test.shape[1]);

    // within the native tolerance (see PointLonLat::EPS)
    EXPECT(points_equal(grid.first_point(), test.first));
    EXPECT(points_equal(grid.point(1), test.second));
    EXPECT(points_equal(grid.last_point(), test.last));
}


CASE("named grids: points and bounding box") {
    for (const auto& test : TESTS) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.name));

        EXPECT(grid->type() == "regular_xy");
        EXPECT(grid->projection().type() == "lcc");
        EXPECT(grid->order() == "i+j+");  // south-west to north-east

        check_points(*grid, test);

        const auto [n, w, s, e] = grid->boundingBox().deconstruct();
        EXPECT(types::is_approximately_equal(n, test.bbox[0], BBOX_TOLERANCE));
        EXPECT(types::is_approximately_equal(w, test.bbox[1], BBOX_TOLERANCE));
        EXPECT(types::is_approximately_equal(s, test.bbox[2], BBOX_TOLERANCE));
        EXPECT(types::is_approximately_equal(e, test.bbox[3], BBOX_TOLERANCE));

        // also by grid, and from the spec
        std::unique_ptr<const Grid> by_grid(GridFactory::build(spec::Custom{{"grid", test.name}}));
        std::unique_ptr<const Grid> from_spec(GridFactory::make_from_string(grid->spec_str()));

        EXPECT(*by_grid == *grid);
        EXPECT(*from_spec == *grid);
        check_points(*from_spec, test);
    }
}


CASE("named grids: points by PROJ, of the projection's PROJ string") {
    if (!ProjectionFactory::has_type("proj")) {
        return;
    }

    for (const auto& test : TESTS) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.name));

        // the same grid (x, y and false easting/northing), projected by PROJ
        std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(YAMLParser::decodeString(grid->spec_str())));
        spec->set("projection", new spec::Custom{{"type", "proj"}, {"source", grid->projection().proj_str()}});

        std::unique_ptr<const Grid> proj(GridFactory::build(*spec));
        EXPECT(proj->projection().type() == "proj");

        check_points(*proj, test);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
