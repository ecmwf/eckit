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


#include <iostream>
#include <memory>

#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/Composer.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/projection/Mercator.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::util {
area::BoundingBox bounding_box(Point2, Point2, Projection&);
}


namespace eckit::test {


using namespace geo;

using P = std::unique_ptr<Projection>;


CASE("projection: none") {
    Point p = PointLonLat{1, 1};
    P projection(ProjectionFactory::instance().get("none").create(spec::Custom{}));

    EXPECT(points_equal(p, projection->inv(p)));
    EXPECT(points_equal(p, projection->fwd(p)));
}


CASE("projection: plate-caree") {
    Point p = PointLonLat{1, 1};
    Point q = Point2{1, 1};
    P projection(ProjectionFactory::instance().get("plate-carree").create(spec::Custom{}));

    EXPECT(points_equal(q, projection->inv(p)));
    EXPECT(std::holds_alternative<Point2>(projection->inv(p)));

    EXPECT(points_equal(p, projection->fwd(q)));
    EXPECT(std::holds_alternative<PointLonLat>(projection->fwd(q)));
}


CASE("projection: ll_to_xyz") {
    Point p = PointLonLat{1, 1};
    P s1(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"R", 1.}})));
    P s2(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 1.}})));
    P s3(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 0.5}})));

    EXPECT(points_equal(p, s1->inv(s1->fwd(p))));
    EXPECT(points_equal(p, s2->inv(s2->fwd(p))));
    EXPECT(points_equal(s1->fwd(p), s2->fwd(p)));

    Point q = PointLonLat{1, 0};

    EXPECT(points_equal(s1->fwd(q), s3->fwd(q)));
    EXPECT(points_equal(s2->fwd(q), s3->fwd(q)));

    struct {
        PointLonLat a;
        Point3 b;
    } tests[] = {
        {{0, 0}, {1, 0, 0}},
        {{90, 0}, {0, 1, 0}},
        {{180, 0}, {-1, 0, 0}},
        {{270, 0}, {0, -1, 0}},
        {{0, -90}, {0, 0, -0.5}},
        {{42, -90}, {0, 0, -0.5}},
        {{0, 90}, {0, 0, 0.5}},
        {{42, 90}, {0, 0, 0.5}},
    };

    for (const auto& test : tests) {
        EXPECT(points_equal(s3->fwd(test.a), test.b));
    }
}


CASE("projection: ll_to_xyz") {
    const PointLonLat p(723., 1.);  // <- FIXME

    projection::LonLatToXYZ to_xyz_r(1.);

    auto q = to_xyz_r.fwd(p);
    auto r = to_xyz_r.inv(q);
    Log::info() << "p(lat, lon): " << p << " -> p(x,y,z): " << q << " -> p(lat, lon): " << r << std::endl;

    EXPECT(points_equal(p, r));

    // oblate spheroid (supported)
    projection::LonLatToXYZ to_xyz_ab(3., 2.);

    for (const auto& lon : {0., 90., 180., 270.}) {
        PointLonLat p{lon, 0.};
        Log::info() << "p(lat, lon): " << p << " -> p_ab(x,y,z): " << to_xyz_ab.fwd(p) << std::endl;
    }

    // problate spheroid (not supported)
    EXPECT_THROWS(projection::LonLatToXYZ(2., 3.));
}


CASE("projection: rotation") {
    constexpr double eps = 1e-6;


    SECTION("rotation (1)") {
        spec::Custom spec({
            {"projection", "rotation"},
            {"south_pole_lat", -91.},
            {"south_pole_lon", -361.},
        });

        Point p = PointLonLat{1, 1};
        P projection(ProjectionFactory::instance().get(spec.get_string("projection")).create(spec));

        EXPECT(points_equal(p, projection->inv(projection->fwd(p))));
        EXPECT(points_equal(p, projection->fwd(projection->inv(p))));
    }


    SECTION("rotation (2)") {
        const PointLonLat p(1, 1);
        int delta[] = {-360, -180, -1, 0, 1, 90, 91, 180};

        for (auto a : delta) {
            for (auto b : delta) {
                for (auto c : delta) {
                    projection::Rotation rot(0. + static_cast<double>(b),
                                             -90. + static_cast<double>(a),
                                             static_cast<double>(c));
                    EXPECT(rot.rotated() == (a % 360 != 0 || (b - c) % 360 != 0));

                    EXPECT(points_equal(p, rot.inv(rot.fwd(p)), eps));
                    EXPECT(points_equal(p, rot.fwd(rot.inv(p)), eps));
                }
            }
        }
    }


    SECTION("rotation (3)") {
        const int Ni = 12;
        const int Nj = 3;

        projection::Rotation rot(182., -46.7, 0.);
        const PointLonLat ref[]{
            {-178., -46.7},  //
            {-178., -16.7},
            {-178., 13.3},
            {-178., 43.3},
            {-178., 73.3},
            {2., 76.7},
            {2., 46.7},
            {-178., -46.7},
            {-162.623427, -19.469294},
            {-152.023657, 8.654593},
            {-139.574639, 36.436827},
            {-113.108943, 61.431994},
            {-39.882454, 68.008245},
            {2., 46.7},
            {-178., -46.7},
            {-148.834426, -27.310675},
            {-129.263456, -3.837005},
            {-110.791162, 20.054216},
            {-85.879167, 41.365070},
            {-44.424956, 53.295076},
            {2., 46.7},
            {-178., -46.7},
            {-137.907940, -39.070023},
            {-109.601456, -21.339065},
            {-88., 0.},
            {-66.398544, 21.339065},
            {-38.092060, 39.070023},
            {2., 46.7},
            {-178., -46.7},
            {-131.575044, -53.295076},
            {-90.120833, -41.365070},
            {-65.208838, -20.054216},
            {-46.736544, 3.837005},
            {-27.165574, 27.310675},
            {2., 46.7},
            {-178., -46.7},
            {-136.117546, -68.008245},
            {-62.891057, -61.431994},
            {-36.425361, -36.436827},
            {-23.976343, -8.654593},
            {-13.376573, 19.469294},
            {2., 46.7},
            {-178., -46.7},
            {-178., -76.7},
            {2., -73.3},
            {2., -43.3},
            {2., -13.3},
            {2., 16.7},
            {2., 46.7},
            {-178., -46.7},
            {140.117546, -68.008245},
            {66.891057, -61.431994},
            {40.425361, -36.436827},
            {27.976343, -8.654593},
            {17.376573, 19.469294},
            {2., 46.7},
            {-178., -46.7},
            {135.575044, -53.295076},
            {94.120833, -41.365070},
            {69.208838, -20.054216},
            {50.736544, 3.837005},
            {31.165574, 27.310675},
            {2., 46.7},
            {-178., -46.7},
            {141.907940, -39.070023},
            {113.601456, -21.339065},
            {92., 0.},
            {70.398544, 21.339065},
            {42.092060, 39.070023},
            {2., 46.7},
            {-178., -46.7},
            {152.834426, -27.310675},
            {133.263456, -3.837005},
            {114.791162, 20.054216},
            {89.879167, 41.365070},
            {48.424956, 53.295076},
            {2., 46.7},
            {-178., -46.7},
            {166.623427, -19.469294},
            {156.023657, 8.654593},
            {143.574639, 36.436827},
            {117.108943, 61.431994},
            {43.882454, 68.008245},
            {2., 46.7},
        };

        for (int i = 0, k = 0; i < Ni; i++) {
            for (int j = 0; j < 2 * Nj + 1; j++, k++) {
                PointLonLat a(static_cast<double>(i) * 360. / static_cast<double>(Ni),
                              static_cast<double>(j - Nj) * 90. / static_cast<double>(Nj));
                auto b = rot.fwd(a);
                auto c = rot.inv(b);

                EXPECT(points_equal(b, ref[k], eps));
                EXPECT(points_equal(a, c, eps));
            }
        }
    }


    SECTION("rotation (4)") {
        const projection::Rotation non_rotated(0., -90., 0.);
        const projection::Rotation rotation_angle(0., -90., -180.);
        const projection::Rotation rotation_matrix(4., -40., 180.);

        EXPECT(not non_rotated.rotated());
        EXPECT(rotation_angle.rotated());
        EXPECT(rotation_matrix.rotated());

        const PointLonLat p[] = {{0., 90.}, {0., 0.}, {270., 25.}, {-180., 45.}};

        struct {
            const projection::Rotation& rotation;
            const PointLonLat a;
            const PointLonLat b;
        } tests[] = {
            {non_rotated, p[0], p[0]},
            {non_rotated, p[1], p[1]},
            {non_rotated, p[2], p[2]},
            {non_rotated, p[3], p[3]},
            {rotation_angle, p[0], {p[0].lon - 180., p[0].lat}},
            {rotation_angle, p[1], {p[1].lon - 180., p[1].lat}},
            {rotation_angle, p[2], {p[2].lon - 180., p[2].lat}},
            {rotation_angle, p[3], {p[3].lon - 180., p[3].lat}},
            {rotation_matrix, p[0], {-176., 40.}},
            {rotation_matrix, p[1], {-176., -50.}},
            {rotation_matrix, p[2], {113.657357, 15.762700}},
            {rotation_matrix, p[3], {-176., 85.}},
        };

        for (const auto& test : tests) {
            auto b = test.rotation.fwd(test.a);
            EXPECT(points_equal(b, test.b, eps));

            auto a = test.rotation.inv(b);
            EXPECT(points_equal(a, test.a, eps));
        }
    }


    SECTION("rotation (5)") {
        spec::Custom spec({
            {"projection", "rotation"},
            {"south_pole_lat", -90.},
            {"south_pole_lon", 0.},
            {"angle", 45.},
        });

        // compose sequentially
        const auto& builder = ProjectionFactory::instance().get("rotation");
        P composition1(new projection::Composer{
            builder.create(spec),
            builder.create(spec),
            builder.create(spec),
            builder.create(spec),
            builder.create(spec),
            builder.create(spec),
            builder.create(spec),
        });

        dynamic_cast<projection::Composer*>(composition1.get())->emplace_back(builder.create(spec));

        for (auto lat : {0., 10., -10.}) {
            PointLonLat p{0., lat};
            auto q = composition1->fwd(p);

            EXPECT(points_equal(p, q));
            EXPECT(points_equal(p, composition1->inv(q)));

            auto qs = dynamic_cast<projection::Composer*>(composition1.get())->fwd_points(p);
            EXPECT(qs.size() == 8);

            EXPECT(points_equal(qs.front(), PointLonLat{-45., lat}));
            EXPECT(points_equal(qs[1], PointLonLat{-90., lat}));
            EXPECT(points_equal(qs[2], PointLonLat{-135., lat}));
            // ...
            EXPECT(points_equal(qs.back(), p));
        }

        // compose by nesting
        P composition2(builder.create(spec));
        for (size_t i = 1; i < 8; ++i) {
            composition2.reset(projection::Composer::compose_back(composition2.release(), spec));
        }

        for (auto lat : {0., 10., -10.}) {
            PointLonLat p{0., lat};

            auto qs1 = dynamic_cast<projection::Composer*>(composition1.get())->fwd_points(p);
            auto qs2 = dynamic_cast<projection::Composer*>(composition2.get())->fwd_points(p);

            ASSERT(qs1.size() == 8);
            EXPECT(qs2.size() == 2);
            EXPECT(points_equal(qs1[6], qs2[0]));
            EXPECT(points_equal(qs1[7], qs2[1]));
        }
    }
}


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
            const bool is_periodic_west_east;
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

            EXPECT_EQUAL(test.is_periodic_west_east, bbox.isPeriodicWestEast());
            EXPECT_EQUAL(test.contains_north_pole, bbox.containsNorthPole());
            EXPECT_EQUAL(test.contains_south_pole, bbox.containsSouthPole());
        }
    }
}


CASE("projection: mercator") {
    PointLonLat first{262.036, 14.7365};


    SECTION("mercator (1)") {
        projection::Mercator projection(0., 14., new figure::Sphere(6371229.), first);

        Point2 a{0., 0.};
        auto b = projection.inv(a);
        auto c = projection.fwd(b);

        EXPECT(points_equal(c, a));
    }


    SECTION("mercator (2)") {
        projection::Mercator projection(-180., 0., new figure::Sphere(1.), {0, 0});

        PointLonLat a{-75., 35.};
        auto b = projection.fwd({-75, 35});
        auto c = projection.inv(b);

        EXPECT(points_equal(c, a));
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}