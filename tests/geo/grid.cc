/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Grid from grid_spec") {
    SECTION("GridSpec canonical") {
        struct {
            const char* grid;
            const char* canonical;
            size_t size;
        } tests[]{
            {"{grid: [10, 10]}", R"({"grid":[10,10]})", 684},          //
            {"{grid: [20, 10]}", R"({"grid":[20,10]})", 342},          //
            {"{pl: [20, 24, 24, 20]}", R"({"grid":"O2"})", 88},        //
            {"{grid: o8}", R"({"grid":"O8"})", 544},                   //
            {"{grid: h2}", R"({"grid":"H2"})", 48},                    //
            {"{grid: h2n}", R"({"grid":"H2","order":"nested"})", 48},  //
        };


        for (const auto& test : tests) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.grid));

            EXPECT(grid->size() == test.size);
            EXPECT(grid->spec_str() == test.canonical);

            static const auto bbox_spec_str = area::BoundingBox::bounding_box_default().spec_str();

            EXPECT(grid->boundingBox().spec_str() == bbox_spec_str);
        }
    }
}


CASE("Grid from name") {
    SECTION("GridFactory::build_from_name") {
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


CASE("Grid from latitudes and longitudes") {
    const std::vector<double> lats{1., 2., 3.};
    const std::vector<double> lons{4., 5., 6.};

    SECTION("keys latitudes/longitudes") {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({
            {"latitudes", lats},
            {"longitudes", lons},
        })));

        EXPECT_EQUAL(grid->size(), lats.size());

        const auto [out_lats, out_lons] = grid->to_latlons();
        EXPECT(out_lats == lats);
        EXPECT(out_lons == lons);

        EXPECT_THROWS((void)GridFactory::build(spec::Custom({{"latitudes", lats}})));
        EXPECT_THROWS((void)GridFactory::build(spec::Custom({{"longitudes", lons}})));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
