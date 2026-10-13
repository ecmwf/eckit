// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("RegularXY: x and y") {
    // 'x' and 'y' are [start, stop, step], stop included
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1]}"));

    const std::vector<size_t> shape{4, 3};

    EXPECT(grid->type() == "regular_xy");
    EXPECT(grid->shape() == shape);
    EXPECT(grid->spec_str() == R"({"x":[0,2,1],"y":[3,0,-1]})");

    // without a projection, the grid points are its (x, y)
    const auto points = grid->to_points();
    EXPECT(points_equal(points.front(), PointXY{0., 3.}));
    EXPECT(points_equal(points[1], PointXY{1., 3.}));
    EXPECT(points_equal(points.back(), PointXY{2., 0.}));

    SECTION("order, as the directions of x and y (unless the default)") {
        struct test_t {
            std::string spec;
            std::string spec_str;
        };

        for (const auto& test : std::vector<test_t>{
                 {"{x: [0, 2, 1], y: [0, 3, 1]}", R"({"order":"i+j+","x":[0,2,1],"y":[0,3,1]})"},
                 {"{x: [2, 0, -1], y: [3, 0, -1]}", R"({"order":"i-j-","x":[2,0,-1],"y":[3,0,-1]})"},
                 {"{x: [0, 2, 1], y: [0, 3, 1], order: j+i+}", R"({"order":"j+i+","x":[0,2,1],"y":[0,3,1]})"},
             }) {
            std::unique_ptr<const Grid> g(GridFactory::make_from_string(test.spec));
            EXPECT(g->spec_str() == test.spec_str);
        }

        EXPECT_THROWS_AS(GridFactory::make_from_string("{x: [0, 2, 1], y: [0, 3, 1], order: i+j-}"),
                         exception::SpecError);
    }

    SECTION("false easting/northing") {
        std::unique_ptr<const Grid> with_false(
            GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1], false: [10, 20]}"));
        EXPECT(with_false->spec_str() == R"({"false":[10,20],"x":[0,2,1],"y":[3,0,-1]})");
        EXPECT(points_equal(with_false->first_point(), PointXY{0., 3.}));

        std::unique_ptr<const Grid> without_false(
            GridFactory::make_from_string("{x: [0, 2, 1], y: [3, 0, -1], false: [0, 0]}"));
        EXPECT(without_false->spec_str() == grid->spec_str());
    }
}


CASE("RegularXY: x and y, any step") {
    // the stop is included to a small fraction of the step, so it can be as written (to 15 significant digits)
    for (const auto& [spec, size, last] : std::vector<std::tuple<std::string, size_t, PointXY>>{
             {"{x: [0, 29974.0379635747, 2997.40379635747], y: [0, 1, 1]}", 22, {29974.0379635747, 1.}},
             {"{x: [0, 33005.841, 3000.531], y: [0, -1067074.4, -1067074.4]}", 24, {33005.841, -1067074.4}},
             {"{x: [0, 33005.8361490001, 3000.53055900001], y: [0, -1, -1]}", 24, {33005.8361490001, -1.}},
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
        EXPECT(grid->size() == size);
        EXPECT(points_equal(grid->to_points().back(), last, 1e-6));
    }
}


CASE("RegularXY: reorder") {
    // x and y increasing, so the grid order is i+j+
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{x: [0, 2, 1], y: [0, 1, 1]}"));

    EXPECT(grid->size() == 6);
    EXPECT(grid->order() == "i+j+");

    size_t n = 0;
    for (const auto& p : *grid) {
        EXPECT(std::holds_alternative<PointXY>(p));
        ++n;
    }
    EXPECT(n == 6);

    const auto points = grid->to_points();
    EXPECT(points.size() == 6);

    // points in each order, the empty order being the default (i+j-)
    for (const auto& [order, expected] : std::vector<std::pair<std::string, std::vector<PointXY>>>{
             {"", {{0, 1}, {1, 1}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}},
             {"i+j-", {{0, 1}, {1, 1}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}},
             {"i+j+", {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}}},
             {"i-j-", {{2, 1}, {1, 1}, {0, 1}, {2, 0}, {1, 0}, {0, 0}}},
             {"i-j+", {{2, 0}, {1, 0}, {0, 0}, {2, 1}, {1, 1}, {0, 1}}},
             {"j+i-", {{2, 0}, {2, 1}, {1, 0}, {1, 1}, {0, 0}, {0, 1}}},
             {"j+i+", {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}}},
             {"j-i-", {{2, 1}, {2, 0}, {1, 1}, {1, 0}, {0, 1}, {0, 0}}},
             {"j-i+", {{0, 1}, {0, 0}, {1, 1}, {1, 0}, {2, 1}, {2, 0}}},
         }) {
        // grid point k is point ren[k] in the order
        const auto ren = grid->reorder(order);
        EXPECT(ren.size() == 6);

        for (size_t k = 0; k < points.size(); ++k) {
            EXPECT(points_equal(points[k], expected[ren[k]]));
        }
    }
}


CASE("RegularXY: alternating rows (or columns)") {
    // rows (or columns) scanning in alternate directions, as GRIB alternativeRowScanning
    for (const auto& [order, expected] : std::vector<std::pair<std::string, std::vector<PointXY>>>{
             {"i+-j+", {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {1, 1}, {0, 1}}},
             {"j+-i+", {{0, 0}, {0, 1}, {1, 1}, {1, 0}, {2, 0}, {2, 1}}},
         }) {
        std::unique_ptr<const Grid> grid(
            GridFactory::make_from_string("{x: [0, 2, 1], y: [0, 1, 1], order: " + order + "}"));

        EXPECT(grid->order() == order);
        EXPECT(grid->size() == 6);

        const auto points = grid->to_points();
        EXPECT(points.size() == expected.size());
        for (size_t k = 0; k < expected.size(); ++k) {
            EXPECT(points_equal(points[k], expected[k]));
        }

        // grid point k is point ren[k] in the order i+j+
        const auto ren = grid->reorder("i+j+");

        const std::vector<PointXY> ipos_jpos{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}};
        for (size_t k = 0; k < points.size(); ++k) {
            EXPECT(points_equal(points[k], ipos_jpos[ren[k]]));
        }
    }
}


CASE("RegularXY: projection on top") {
    // (x, y) to (lon, lat), applied to the grid's (x, y) less the false easting/northing
    const std::string projected =
        "{x: [0, 20, 10], y: [0, 10, 10], false: [10, 10], projection: {type: eqc, lat_ts: 60, reverse: true}}";

    std::unique_ptr<const Grid> grid(GridFactory::make_from_string(projected));

    EXPECT(
        grid->spec_str() ==
        R"({"false":[10,10],"order":"i+j+","projection":{"lat_ts":60,"reverse":true,"type":"eqc"},"x":[0,20,10],"y":[0,10,10]})");

    const auto points = grid->to_points();
    EXPECT(points_equal(points.front(), PointLonLat{-20., -10.}));
    EXPECT(points_equal(points[1], PointLonLat{0., -10.}));
    EXPECT(points_equal(points.back(), PointLonLat{20., 0.}));

    const auto [lat, lon] = grid->to_latlons();
    EXPECT(lon.size() == grid->size());

    // outwards, to its precision
    const auto& bbox = grid->boundingBox();
    const auto h     = area::BoundingBox::PRECISION_LL;

    EXPECT(types::is_approximately_equal(bbox.north(), 0. + h, PointLonLat::EPS));
    EXPECT(types::is_approximately_equal(bbox.south(), -10. - h, PointLonLat::EPS));
    EXPECT(types::is_approximately_equal(bbox.west(), -20. - h, PointLonLat::EPS));
    EXPECT(types::is_approximately_equal(bbox.east(), 20. + h, PointLonLat::EPS));

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


CASE("RegularXY: as GRIB") {
    // grids as GRIB examples (by eccodes): the false easting/northing places the first grid point at (0, 0); points
    // are as GRIB, to microdegrees
    constexpr double microdegree = 1e-6;

    struct test_t {
        std::string name;
        std::string xy;
        std::string projection;
        std::string proj;
        std::string spec_str;
        std::vector<PointLonLat> first_second_last;
    };

    for (
        const auto& test : std::vector<test_t>{
            // gridType=lambert_lam (as lambert), Nx=49, Ny=69, Dx=Dy=2500 m, LoV=4.9, LaD=Latin1=Latin2=51.967
            {"lambert_lam.grib2",
             "x: [0, 120000, 2500], y: [0, 170000, 2500], false: [60000.021, 85000.055]",
             "type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9",
             "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229",
             R"({"false":[60000.021,85000.055],"order":"i+j+",)"
             R"("projection":{"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true,"type":"lcc"},)"
             R"("x":[0,120000,2500],"y":[0,170000,2500]})",
             {{4.038959, 51.199433}, {4.074832551, 51.199693547}, {5.790918449, 52.728068627}}},
            // gridType=lambert, Nx=181, Ny=139, Dx=Dy=8000 m, LoV=308.65, LaD=Latin1=Latin2=5.12, jScansPositively=0
            // (points as PROJ, the eccodes iterator assumes latitudes increasing from the first grid point)
            {"PFFPOS000+0000_guyane.grb",
             "x: [0, 1440000, 8000], y: [0, -1104000, -8000], false: [720000.044, -552000.009]",
             "type: lcc, lat_0: 5.12, lat_1: 5.12, lon_0: 308.65",
             "+proj=lcc +lat_0=5.12 +lat_1=5.12 +lon_0=308.65 +R=6371229",
             R"({"false":[720000.044,-552000.009],)"
             R"("projection":{"lat_0":5.12,"lat_1":5.12,"lon_0":308.65,"reverse":true,"type":"lcc"},)"
             R"("x":[0,1440000,8000],"y":[0,-1104000,-8000]})",
             {{302.098546001, 10.044953002}, {302.17133496, 10.045680339}, {315.100527634, 0.129704986}}},
            // gridType=mercator, Ni=56, Nj=44, Di=Dj=45000 m, LaD=14
            {"test_mercator.temp.grib2",
             "x: [0, 2475000, 45000], y: [0, 1935000, 45000], false: [10569854.248, -1607827.789]",
             "type: merc, lat_ts: 14",
             "+proj=merc +lat_ts=14 +R=6371229",
             R"({"false":[10569854.248,-1607827.789],"order":"i+j+","projection":{"lat_ts":14,"reverse":true,"type":"merc"},)"
             R"("x":[0,2475000,45000],"y":[0,1935000,45000]})",
             {{262.036499, 14.736453}, {262.453567906, 14.736453}, {284.975288815, 31.173065977}}},
            // gridType=polar_stereographic, Nx=765, Ny=570, Dx=Dy=2500 m, LaD=60, LoV=280, projectionCentreFlag=0
            {"CMC_hrdps_east_TMP_TGL_2_ps2.5km_2014081512_P000-00.grib2",
             "x: [0, 1910000, 2500], y: [0, 1422500, 2500], false: [1122822.896, 5597229.258]",
             "type: stere, lat_0: 90, lat_ts: 60, lon_0: 280",
             "+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229",
             R"({"false":[1122822.896,5597229.258],"order":"i+j+",)"
             R"("projection":{"lat_0":90,"lat_ts":60,"lon_0":280,"reverse":true,"type":"stere"},)"
             R"("x":[0,1910000,2500],"y":[0,1422500,2500]})",
             {{268.656832, 38.7017}, {268.681435261, 38.705547328}, {290.67818439, 50.672962595}}},
            // gridType=lambert_azimuthal_equal_area, Nx=1000, Ny=950, Dx=Dy=5000 m, standardParallel=52,
            // centralLongitude=10, shapeOfTheEarth=4 (GRS80), jScansPositively=0
            {"opera.grib2",
             "x: [0, 4995000, 5000], y: [0, -4745000, -5000], false: [1818500.021, -2287499.955]",
             "type: laea, lat_0: 52, lon_0: 10, figure: grs80",
             "+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80",
             R"({"false":[1818500.021,-2287499.955],)"
             R"("projection":{"figure":"grs80","lat_0":52,"lon_0":10,"reverse":true,"type":"laea"},)"
             R"("x":[0,4995000,5000],"y":[0,-4745000,-5000]})",
             {{-35.034024, 66.982142999}, {-34.944263537, 67.009598344}, {41.139704951, 23.942342883}}},
        }) {
        SECTION(test.name) {
            auto check = [&test](const std::string& projection) {
                std::unique_ptr<const Grid> grid(
                    GridFactory::make_from_string("{" + test.xy + ", projection: {" + projection + "}}"));

                const auto points = grid->to_points();
                EXPECT(points_equal(points.front(), test.first_second_last[0], microdegree));
                EXPECT(points_equal(points[1], test.first_second_last[1], microdegree));
                EXPECT(points_equal(points.back(), test.first_second_last[2], microdegree));

                return grid->spec_str();
            };

            EXPECT(check(test.projection + ", reverse: true") == test.spec_str);

            // without projection, the grid points are on the plane
            std::unique_ptr<const Grid> plane(GridFactory::make_from_string("{" + test.xy + "}"));
            EXPECT(points_equal(plane->first_point(), PointXY{0., 0.}));

            // from its own spec, the same grid
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.spec_str));
            EXPECT(grid->spec_str() == test.spec_str);

            if (ProjectionFactory::has_type("proj")) {
                // PROJ string from the grid, and from its projection spec
                EXPECT(grid->projection().proj_str() == test.proj);
                std::unique_ptr<const Projection> projection(
                    ProjectionFactory::make_from_string("{" + test.projection + "}"));
                EXPECT(projection->proj_str() == test.proj);
                check("type: proj, source: '" + test.proj + "'");
            }
        }
    }
}


CASE("RegularXY: space_view, as GRIB") {
    // grids as GRIB (by eccodes): Nr=6.6107, dx=3622, dy=3622 (or 3610), Xp=Yp=1856, sectors (Xo, Yo) of 16 x 16 (or
    // 12) points; points as the eccodes iterator (and PROJ geos), to microdegrees
    constexpr double microdegree = 1e-6;

    for (
        const auto& [spec_str, first_second_last] : std::vector<std::pair<std::string, std::vector<PointLonLat>>>{
            {R"({"false":[1067074.48,-467594.435],"projection":{"h":35747054.5503,"reverse":true,"type":"space_view"},)"
             R"("x":[0,44961.0033558134,2997.40022372089],"y":[0,-44961.0033558134,-2997.40022372089]})",
             {{350.299524805, 4.223693723}, {350.327193534, 4.223622147}, {350.719698129, 3.815601110}}},
            // iScansNegatively=1, jScansPositively=1 (as MSG)
            {R"({"false":[-1022113.476,422633.432],"projection":{"h":35747054.5503,"reverse":true,"type":"space_view"},)"
             R"("x":[0,44961.0033558134,2997.40022372089],"y":[0,-44961.0033558134,-2997.40022372089]})",
             {{9.280301871, -3.815601110}, {9.307903835, -3.815663064}, {9.700475195, -4.223693723}}},
            {R"({"false":[-1632353.683,-1968348.046],)"
             R"("projection":{"figure":"wgs84","h":35785813.2659,"lon_0":-75,"reverse":true,"type":"space_view"},)"
             R"("x":[0,45009.7522881123,3000.65015254082],"y":[0,-33005.8361442917,-3000.53055857197]})",
             {{300.905565350, 18.425011761}, {300.936080607, 18.425558651}, {301.327135202, 18.106822780}}},
        }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec_str));
        EXPECT(grid->spec_str() == spec_str);

        const auto points = grid->to_points();
        EXPECT(points_equal(points.front(), first_second_last[0], microdegree));
        EXPECT(points_equal(points[1], first_second_last[1], microdegree));
        EXPECT(points_equal(points.back(), first_second_last[2], microdegree));

        // not a PROJ projection
        EXPECT_THROWS(grid->projection().proj_str());
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

        EXPECT(grid->type() == "regular_xy");
        EXPECT(grid->shape() == shape);
        EXPECT(grid->size() == 69 * 33);

        // a geographic bounding box, unlike the spec's own frame
        EXPECT(grid->boundingBox().north() < NORTH_POLE.lat());
    }


    SECTION("spec round-trip") {
        // NOTE: without 'type' a two-valued 'grid' would be taken for a regular_ll's increments, in degrees
        for (const auto& name : std::vector<std::string>{"swisslv95", "swisslv03"}) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(name));
            std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));

            EXPECT(same->spec_str() == grid->spec_str());
            EXPECT(same->shape() == grid->shape());
        }
    }


    SECTION("swisslv95 isn't snapped to multiples of its increment") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("swisslv95"));

        const std::vector<size_t> shape{294, 429};

        EXPECT(grid->shape() == shape);
        EXPECT(grid->spec().get_double_vector("y")[0] == 1040500.);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
