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


#include <memory>

#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::util {
area::BoundingBox bounding_box(Point2, Point2, Projection&);
}


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


CASE("projection: proj") {
    constexpr double eps = 1e-6;


    if (ProjectionFactory::instance().exists("proj")) {
        PointLonLat a{12., 55.};

        struct {
            const Point b;
            const std::string target;
        } tests_proj[] = {
            {Point2{691875.632137542, 6098907.825129169}, "+proj=utm +zone=32 +datum=WGS84"},
            {Point2{691875.632137542, 6098907.825129169}, "EPSG:32632"},
            {a, "EPSG:4326"},
            {a, "EPSG:4979"},
            {Point3{3586469.6567764, 762327.65877826, 5201383.5232023}, "EPSG:4978"},
            {Point3{3574529.7050235, 759789.74368715, 5219005.2599833}, "+proj=cart +R=6371229."},
            {Point3{3574399.5431832, 759762.07693392, 5218815.216709}, "+proj=cart +ellps=sphere"},
            {a, "+proj=latlon +ellps=sphere"},
        };

        for (const auto& test : tests_proj) {
            P projection(ProjectionFactory::instance().get("proj").create(
                spec::Custom{{{"source", "EPSG:4326"}, {"target", test.target}}}));

#if 0
    Log::info() << "ellipsoid: '" << PROJ::ellipsoid(projection.target())
              << std::endl;
#endif

            auto b = projection->fwd(a);
            auto c = projection->inv(b);

            EXPECT(points_equal(b, test.b, eps));
            EXPECT(points_equal(c, a, eps));

            P reverse(ProjectionFactory::instance().get("proj").create(
                spec::Custom({{"source", test.target}, {"target", "EPSG:4326"}})));

            auto d = reverse->fwd(test.b);
            auto e = reverse->inv(d);

            EXPECT(points_equal(d, a, eps));
            EXPECT(points_equal(e, test.b, eps));
        }

        P polar_stereographic_north(ProjectionFactory::instance().get("proj").create(
            spec::Custom{{{"source", "EPSG:4326"}, {"target", "+proj=stere +lat_0=90. +lon_0=-30. +R=6371229."}}}));

        P polar_stereographic_south(ProjectionFactory::instance().get("proj").create(
            spec::Custom{{{"source", "EPSG:4326"}, {"target", "+proj=stere +lat_0=-90. +lon_0=-30. +R=6371229."}}}));

        struct {
            const P& projection;
            const Point2 min;
            const Point2 max;
            const bool periodic;
            const bool contains_north_pole;
            const bool contains_south_pole;
        } tests_bbox[] = {
            {polar_stereographic_north, {-2e6, -2e6}, {2e6, 2e6}, true, true, false},
            {polar_stereographic_north, {-2e6, -2e6}, {1e6, 1e6}, true, true, false},
            {polar_stereographic_north, {-2e6, -2e6}, {-1e6, -1e6}, false, false, false},
            {polar_stereographic_north, {-1e6, -1e6}, {2e6, 2e6}, true, true, false},
            {polar_stereographic_north, {-1e6, -1e6}, {1e6, 1e6}, true, true, false},
            {polar_stereographic_north, {1e6, 1e6}, {2e6, 2e6}, false, false, false},
            {polar_stereographic_south, {-2e6, -2e6}, {2e6, 2e6}, true, false, true},
            {polar_stereographic_south, {-2e6, -2e6}, {1e6, 1e6}, true, false, true},
            {polar_stereographic_south, {-2e6, -2e6}, {-1e6, -1e6}, false, false, false},
            {polar_stereographic_south, {-1e6, -1e6}, {2e6, 2e6}, true, false, true},
            {polar_stereographic_south, {-1e6, -1e6}, {1e6, 1e6}, true, false, true},
            {polar_stereographic_south, {1e6, 1e6}, {2e6, 2e6}, false, false, false},
        };

        for (const auto& test : tests_bbox) {
            auto bbox = util::bounding_box(test.min, test.max, *test.projection);

            EXPECT_EQUAL(test.periodic, bbox.periodic());
            EXPECT_EQUAL(test.contains_north_pole, bbox.contains(NORTH_POLE));
            EXPECT_EQUAL(test.contains_south_pole, bbox.contains(SOUTH_POLE));

            auto global = test.periodic && test.contains_north_pole && test.contains_south_pole;

            EXPECT(global == bbox.global());
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
