// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
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

    for (const auto& test : std::vector<test_t>{
             // gridType=lambert_lam (as lambert), Nx=49, Ny=69, Dx=Dy=2500 m, LoV=4.9, LaD=Latin1=Latin2=51.967
             {"lambert_lam.grib2",
              "x: [0, 120000, 2500], y: [0, 170000, 2500], false: [60000.021, 85000.055]",
              "type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9",
              "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229",
              R"({"false":[60000.021,85000.055],)"
              R"("projection":{"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"reverse":true,"type":"lcc"},)"
              R"("x":[0,120000,2500],"y":[0,170000,2500]})",
              {{4.038959, 51.199433}, {4.074832551, 51.199693547}, {5.790918449, 52.728068627}}},
             // gridType=mercator, Ni=56, Nj=44, Di=Dj=45000 m, LaD=14
             {"test_mercator.temp.grib2",
              "x: [0, 2475000, 45000], y: [0, 1935000, 45000], false: [10569854.248, -1607827.789]",
              "type: merc, lat_ts: 14",
              "+proj=merc +lat_ts=14 +R=6371229",
              R"({"false":[10569854.248,-1607827.789],"projection":{"lat_ts":14,"reverse":true,"type":"merc"},)"
              R"("x":[0,2475000,45000],"y":[0,1935000,45000]})",
              {{262.036499, 14.736453}, {262.453567906, 14.736453}, {284.975288815, 31.173065977}}},
             // gridType=polar_stereographic, Nx=765, Ny=570, Dx=Dy=2500 m, LaD=60, LoV=280, projectionCentreFlag=0
             {"CMC_hrdps_east_TMP_TGL_2_ps2.5km_2014081512_P000-00.grib2",
              "x: [0, 1910000, 2500], y: [0, 1422500, 2500], false: [1122822.896, 5597229.258]",
              "type: stere, lat_0: 90, lat_ts: 60, lon_0: 280",
              "+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229",
              R"({"false":[1122822.896,5597229.258],)"
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

            EXPECT_EQUAL(check(test.projection + ", reverse: true"), test.spec_str);

            // without projection, the grid points are on the plane
            std::unique_ptr<const Grid> plane(GridFactory::make_from_string("{" + test.xy + "}"));
            EXPECT(points_equal(plane->first_point(), PointXY{0., 0.}));

            if (ProjectionFactory::has_type("proj") && LibEcKitGeo::projdb_is_available()) {
                check("type: proj, source: '" + test.proj + "'");
            }
        }
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
