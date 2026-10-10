// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("RegularXY: x and y") {
    // 'x' and 'y' are [start, stop, step], stop included
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1]}"));

    const std::vector<size_t> shape{4, 3};

    EXPECT_EQUAL(grid->type(), "regular_xy");
    EXPECT(grid->shape() == shape);
    EXPECT_EQUAL(grid->spec_str(), R"({"x":[0,2,1],"y":[3,0,-1]})");

    // without a projection, the grid points are its (x, y)
    const auto points = grid->to_points();
    EXPECT(points_equal(points.front(), PointXY{0., 3.}));
    EXPECT(points_equal(points[1], PointXY{1., 3.}));
    EXPECT(points_equal(points.back(), PointXY{2., 0.}));

    SECTION("false easting/northing") {
        std::unique_ptr<const Grid> with_false(
            GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1], false: [10, 20]}"));
        EXPECT_EQUAL(with_false->spec_str(), R"({"false":[10,20],"x":[0,2,1],"y":[3,0,-1]})");
        EXPECT(points_equal(with_false->first_point(), PointXY{0., 3.}));

        std::unique_ptr<const Grid> without_false(
            GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1], false: [0, 0]}"));
        EXPECT_EQUAL(without_false->spec_str(), grid->spec_str());
    }
}


CASE("RegularXY: projection on top") {
    // (x, y) to (lon, lat), applied to the grid's (x, y) less the false easting/northing
    const std::string projected =
        "{x: [0, 20, 10], y: [0, 10, 10], false: [10, 10], projection: {type: eqc, lat_ts: 60, reverse: true}}";

    std::unique_ptr<const Grid> grid(GridFactory::make_from_string(projected));

    EXPECT_EQUAL(
        grid->spec_str(),
        R"({"false":[10,10],"projection":{"lat_ts":60,"reverse":true,"type":"eqc"},"x":[0,20,10],"y":[0,10,10]})");

    const auto points = grid->to_points();
    EXPECT(points_equal(points.front(), PointLonLat{-20., -10.}));
    EXPECT(points_equal(points[1], PointLonLat{0., -10.}));
    EXPECT(points_equal(points.back(), PointLonLat{20., 0.}));

    const auto [lat, lon] = grid->to_latlons();
    EXPECT_EQUAL(lon.size(), grid->size());

    const auto& bbox = grid->boundingBox();
    EXPECT(types::is_approximately_equal(bbox.north(), 0., 1e-6));
    EXPECT(types::is_approximately_equal(bbox.south(), -10., 1e-6));
    EXPECT(types::is_approximately_equal(bbox.west(), -20., 1e-6));
    EXPECT(types::is_approximately_equal(bbox.east(), 20., 1e-6));

    // identical, if built from its spec
    std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));
    EXPECT(*same == *grid);

    SECTION("a projection has to take (x, y)") {
        EXPECT_THROWS_AS(GridFactory::make_from_string("{x: [0, 20, 10], y: [0, 10, 10], projection: {type: eqc}}"),
                         exception::SpecError);
    }

    SECTION("a projection has no false easting/northing (the grid has)") {
        EXPECT_THROWS_AS(GridFactory::make_from_string(
                             "{x: [0, 20, 10], y: [0, 10, 10], projection: {type: eqc, reverse: true, x_0: 1}}"),
                         exception::SpecError);
    }
}


CASE("RegularXY") {
    if (!ProjectionFactory::has_type("proj")) {
        return;
    }


    SECTION("EPSG:8857 (Equal Earth), global and coarse") {
        // the grid is defined on the projection plane, in metres
        std::unique_ptr<const Grid> grid(
            GridFactory::make_from_string(R"({"type": "regular_xy", "grid": [500000.0, 500000.0],)"
                                          R"( "bounding_box_xy": [-17000000.0, -8000000.0, 17000000.0, 8000000.0],)"
                                          R"( "projection": {"type": "proj", "source": "EPSG:8857"}})"));

        const std::vector<size_t> shape{33, 69};

        EXPECT_EQUAL(grid->type(), "regular_xy");
        EXPECT(grid->shape() == shape);
        EXPECT_EQUAL(grid->size(), 69 * 33);

        // a geographic bounding box, unlike the spec's own frame
        EXPECT(grid->boundingBox().north() < NORTH_POLE.lat());
    }


    SECTION("spec round-trip") {
        // NOTE: without 'type' a two-valued 'grid' would be taken for a regular_ll's increments, in degrees
        for (const auto& name : std::vector<std::string>{"swisslv95", "swisslv03"}) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(name));
            std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));

            EXPECT_EQUAL(same->spec_str(), grid->spec_str());
            EXPECT(same->shape() == grid->shape());
        }
    }


    SECTION("swisslv95 isn't snapped to multiples of its increment") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("swisslv95"));

        const std::vector<size_t> shape{294, 429};

        EXPECT(grid->shape() == shape);
        EXPECT_EQUAL(grid->spec().get_double_vector("y")[0], 1040500.);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
