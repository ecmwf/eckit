// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/regular/IrregularLL.h"
#include "eckit/geo/range/Irregular.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::regular::IrregularLL;


CASE("range: irregular") {
    range::Irregular inc({-10., 0., 5., 30.});

    EXPECT(inc.increasing());
    EXPECT_EQUAL(inc.size(), 4);
    EXPECT_EQUAL(inc.a(), -10.);
    EXPECT_EQUAL(inc.b(), 30.);
    EXPECT_EQUAL(inc.min(), -10.);
    EXPECT_EQUAL(inc.max(), 30.);

    range::Irregular dec({30., 5., 0.});

    EXPECT(!dec.increasing());
    EXPECT_EQUAL(dec.min(), 0.);
    EXPECT_EQUAL(dec.max(), 30.);

    std::unique_ptr<range::Irregular> cropped(inc.make_cropped_range(1., 30.));
    EXPECT(cropped->values() == std::vector<double>({5., 30.}));

    std::unique_ptr<range::Irregular> cropped_dec(dec.make_cropped_range(10., -1.));
    EXPECT(cropped_dec->values() == std::vector<double>({5., 0.}));

    EXPECT(range::Irregular({1.}).increasing());

    EXPECT_THROWS_AS(range::Irregular({}), exception::RangeError);
    EXPECT_THROWS_AS(range::Irregular({0., 1., 1.}), exception::RangeError);
    EXPECT_THROWS_AS(range::Irregular({0., 2., 1.}), exception::RangeError);
    EXPECT_THROWS_AS(inc.make_cropped_range(31., 40.), exception::RangeError);
}


CASE("grid: irregular_ll") {
    const std::vector<double> lats{60., 50., 45., 30.};
    const std::vector<double> lons{-10., 0., 15.};


    SECTION("points, as the latitudes and longitudes given") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(
            R"({"type":"irregular_ll","latitudes":[60,50,45,30],"longitudes":[-10,0,15]})"));

        EXPECT_EQUAL(grid->type(), "irregular_ll");
        EXPECT_EQUAL(grid->size(), 12);
        EXPECT_EQUAL(grid->nx(), 3);
        EXPECT_EQUAL(grid->ny(), 4);
        EXPECT_EQUAL(grid->order(), "i+j-");

        size_t n = 0;
        for (const auto& p : *grid) {
            const auto& q = std::get<PointLonLat>(p);
            EXPECT(points_equal(q, PointLonLat{lons[n % 3], lats[n / 3]}));
            ++n;
        }
        EXPECT_EQUAL(n, 12);

        const auto bbox = grid->boundingBox();
        EXPECT(bbox == area::BoundingBox(60., -10., 30., 15.));
    }


    SECTION("order, as the directions") {
        struct test_t {
            std::string spec;
            std::string order;
        };

        const std::vector<test_t> cases{
            {R"({"type":"irregular_ll","latitudes":[60,50],"longitudes":[-10,0,15]})", "i+j-"},
            {R"({"type":"irregular_ll","latitudes":[50,60],"longitudes":[-10,0,15]})", "i+j+"},
            {R"({"type":"irregular_ll","latitudes":[60,50],"longitudes":[15,0,-10]})", "i-j-"},
            {R"({"type":"irregular_ll","latitudes":[50,60],"longitudes":[15,0,-10]})", "i-j+"},
            {R"({"type":"irregular_ll","latitudes":[60,50],"longitudes":[-10,0,15],"order":"j-i+"})", "j-i+"},
        };
        for (const auto& test : cases) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.spec));
            EXPECT_EQUAL(grid->order(), test.order);
        }

        // the order and the directions differ
        EXPECT_THROWS_AS(GridFactory::make_from_string(
                             R"({"type":"irregular_ll","latitudes":[60,50],"longitudes":[-10,0,15],"order":"i+j+"})"),
                         exception::SpecError);
    }


    SECTION("spec round trip") {
        const std::vector<std::string> specs{
            R"({"latitudes":[60,50,45,30],"longitudes":[-10,0,15],"type":"irregular_ll"})",
            R"({"latitudes":[30,45],"longitudes":[15,0],"order":"i-j+","type":"irregular_ll"})",
        };
        for (const std::string& spec : specs) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
            EXPECT_EQUAL(grid->spec_str(), spec);

            std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));
            EXPECT(*grid == *same);
        }
    }


    SECTION("cropping") {
        IrregularLL grid(lons, lats);

        std::unique_ptr<const Grid> cropped(grid.make_grid_cropped(area::BoundingBox(55., -5., 40., 20.)));
        EXPECT_EQUAL(cropped->type(), "irregular_ll");
        EXPECT_EQUAL(cropped->spec_str(), R"({"latitudes":[50,45],"longitudes":[0,15],"type":"irregular_ll"})");
    }


    SECTION("errors") {
        const std::vector<std::string> specs{
            R"({"type":"irregular_ll","latitudes":[60,50]})",
            R"({"type":"irregular_ll","longitudes":[-10,0]})",
        };
        for (const auto& spec : specs) {
            EXPECT_THROWS_AS(GridFactory::make_from_string(spec), exception::SpecError);
        }

        EXPECT_THROWS_AS(
            GridFactory::make_from_string(R"({"type":"irregular_ll","latitudes":[60,50,55],"longitudes":[-10,0]})"),
            exception::RangeError);
        EXPECT_THROWS_AS(
            GridFactory::make_from_string(R"({"type":"irregular_ll","latitudes":[100,50],"longitudes":[-10,0]})"),
            exception::SpecError);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
