// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Grid from spec/uid") {
    struct {
        const char* grid;
        const char* canonical;
        size_t size;
    } tests[]{
        {"{grid: [10, 10]}", R"({"grid":[10,10]})", 684},                                  //
        {"{grid: [10, 10], figure: earth}", R"({"grid":[10,10]})", 684},                   //
        {"{grid: [10, 10], figure: wgs84}", R"({"figure":"wgs84","grid":[10,10]})", 684},  //
        {"{grid: [20, 10]}", R"({"grid":[20,10]})", 342},                                  //
        {"{pl: [20, 24, 24, 20]}", R"({"grid":"O2"})", 88},                                //
        {"{grid: o8}", R"({"grid":"O8"})", 544},                                           //
        {"{grid: h2}", R"({"grid":"H2"})", 48},                                            //
        {"{grid: h2n}", R"({"grid":"H2","order":"nested"})", 48},                          //
        {R"({"grid":"F48","rotation":[30,30]})",
         R"({"grid":"F48","projection":{"south_pole":[30,30],"type":"rotation"}})", 48 * 2 * 48 * 4},  //
    };

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.grid));

        EXPECT(grid->size() == test.size);
        EXPECT(grid->spec_str() == test.canonical);

        std::unique_ptr<const Grid> same(GridFactory::build(spec::Custom{{"uid", grid->uid()}}));

        EXPECT(*same == *grid);

        static const auto bbox_spec_str = area::BoundingBox::bounding_box_default().spec_str();

        EXPECT(grid->boundingBox().spec_str() == bbox_spec_str);
    }
}


CASE("Grid name/arrangement") {
    struct {
        const char* grid;
        const char* name;
    } tests[]{
        {"{grid: [10, 10]}", ""},          // unnamed
        {"{pl: [20, 24, 24, 20]}", "O2"},  //
        {"{grid: n32}", "N32"},            //
        {"{grid: o8}", "O8"},              //
        {"{grid: h2}", "H2"},              //
        {"{grid: h2n}", "H2"},             //
        {"{grid: t20}", "T20"},            //
    };

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.grid));

        EXPECT_EQUAL(grid->name(), test.name);
        EXPECT(grid->arrangement().empty());
    }
}


CASE("Grid from name") {
    struct {
        const char* name;
        size_t size;
    } tests[]{{"O2", 88}, {"f2", 32}, {"h2", 48}};

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> a(GridFactory::build(spec::Custom({{"grid", test.name}})));

        EXPECT_EQUAL(test.size, a->size());

        std::unique_ptr<const Grid> b(GridFactory::make_from_string(test.name));

        EXPECT_EQUAL(test.size, b->size());
    }
}


CASE("Grid from increments") {
    using d = std::vector<double>;

    auto prod = [](const std::vector<size_t>& shape) {
        size_t p = 1;
        for (size_t s : shape) {
            p *= s;
        }
        return p;
    };

    struct test_type {
        std::vector<double> grid;
        std::vector<double> area;
        std::vector<size_t> shape;
    };

    for (const auto& test : std::vector<test_type>{
             // global
             {d{1, 1}, d{}, {181, 360}},
             {d{0.05, 0.05}, d{89.975, -179.975, -89.975, 179.975}, {3600, 7200}},

             // non-global
             {d{1, 1}, d{10, 1, 1, 10}, {10, 10}},
             {d{0.25, 0.25}, d{41.0, -4.5, 40.0, -3.0}, {5, 7}},
             {d{0.1, 0.1}, d{41.15, -4.55, 39.95, -3.05}, {13, 16}},
         }) {
        spec::Custom spec{{"grid", test.grid}};
        if (!test.area.empty()) {
            spec.set("area", test.area);
        }

        std::unique_ptr<const Grid> a(GridFactory::build(spec));
        ASSERT(a);

        EXPECT(a->shape() == test.shape);
        EXPECT_EQUAL(a->size(), prod(test.shape));

        std::unique_ptr<const Grid> b(GridFactory::build(a->spec()));
        ASSERT(b);

        EXPECT(*a == *b);
    }
}


CASE("Grid::to_xyz") {
    // points from the projection source, geocentric
    constexpr auto GEOCENTRIC_GRID =
        R"({"type": "regular_xy", "grid": [1000000, 1000000], "bounding_box_xy": [-2000000, -2000000, 2000000, 2000000],)"
        R"( "projection": {"type": "proj", "source": "+proj=geocent +ellps=WGS84", "target": "EPSG:3857"}})";

    // the (lon, lat) grid points, one by one, on the given figure
    auto expect_on_figure = [](const Grid& grid, double a, double b) {
        const projection::LonLatToXYZ to_xyz(a, b);
        const auto [lat, lon] = grid.to_latlons();
        const auto xyz        = grid.to_xyz();

        EXPECT_EQUAL(xyz.size(), 3);
        EXPECT_EQUAL(xyz[0].size(), grid.size());
        for (size_t i = 0; i < grid.size(); ++i) {
            EXPECT(
                points_equal(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, to_xyz.fwd(PointLonLat{lon[i], lat[i]}), 1e-6));
        }
    };


    SECTION("default figure") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(R"({"grid": [30, 30]})"));
        expect_on_figure(*grid, figure::DatumIFS::a, figure::DatumIFS::b);
    }


    SECTION("grid figure") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(R"({"grid": [30, 30], "figure": "wgs84"})"));
        expect_on_figure(*grid, figure::DatumWgs84::a, figure::DatumWgs84::b);
    }


    if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
        return;
    }


    SECTION("PROJ, on the source figure") {
        // EPSG:2056 is on Bessel 1841, its (lon, lat) on WGS 84 (EPSG:4326)
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("swisslv95"));
        EXPECT(std::abs(grid->figure().a() - figure::DatumWgs84::a) > 1.);

        expect_on_figure(*grid, figure::DatumWgs84::a, figure::DatumWgs84::b);
    }


    SECTION("3D grid points, as they are") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(GEOCENTRIC_GRID));
        EXPECT(!grid->empty());

        EXPECT_THROWS_AS(grid->to_latlons(), exception::GridError);

        const auto xyz = grid->to_xyz();
        EXPECT_EQUAL(xyz[0].size(), grid->size());

        size_t i = 0;
        for (const auto& p : *grid) {
            EXPECT(points_equal(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, std::get<PointXYZ>(p)));
            ++i;
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
