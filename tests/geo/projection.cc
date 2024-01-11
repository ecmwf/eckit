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
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/projection/Mercator.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/spec/MappedConfiguration.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using namespace geo;


CASE("projection") {
    using P = std::unique_ptr<Projection>;

    Point p = PointLonLat{1, 1};


    SECTION("projection type: none") {
        P projection(ProjectionFactory::instance().get("none").create(spec::MappedConfiguration{}));
        EXPECT(points_equal(p, projection->inv(p)));
        EXPECT(points_equal(p, projection->fwd(p)));
    }


    SECTION("projection type: rotation") {
        spec::MappedConfiguration param({
            {"projection", "rotation"},
            {"south_pole_lat", -91.},
            {"south_pole_lon", -361.},
        });

        P projection(ProjectionFactory::instance().get(param.getString("projection")).create(param));

        EXPECT(points_equal(p, projection->inv(projection->fwd(p))));
        EXPECT(points_equal(p, projection->fwd(projection->inv(p))));
    }


    SECTION("projection type: ll_to_xyz") {
        P s1(ProjectionFactory::instance().get("ll_to_xyz").create(spec::MappedConfiguration({{"R", 1.}})));
        P s2(ProjectionFactory::instance().get("ll_to_xyz").create(spec::MappedConfiguration({{"a", 1.}, {"b", 1.}})));
        P s3(ProjectionFactory::instance().get("ll_to_xyz").create(spec::MappedConfiguration({{"a", 1.}, {"b", 0.5}})));

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


    SECTION("projection type: ll_to_xyz") {
        const PointLonLat p(723., 1.);  // <- FIXME

        projection::LonLatToXYZ to_xyz_r(1.);

        auto q = to_xyz_r.fwd(p);
        auto r = to_xyz_r.inv(q);
        std::cout << "p(lat, lon): " << p << " -> p(x,y,z): " << q << " -> p(lat, lon): " << r << std::endl;

        EXPECT(points_equal(p, r));

        // oblate spheroid (supported)
        projection::LonLatToXYZ to_xyz_ab(3., 2.);

        for (const auto& lon : {0., 90., 180., 270.}) {
            PointLonLat p{lon, 0.};
            std::cout << "p(lat, lon): " << p << " -> p_ab(x,y,z): " << to_xyz_ab.fwd(p) << std::endl;
        }

        // problate spheroid (not supported)
        EXPECT_THROWS(projection::LonLatToXYZ(2., 3.));
    }


    SECTION("projection type: rotation") {
        const PointLonLat p(1, 1);
        int delta[] = {-360, -180, -1, 0, 1, 90, 91, 180};

        for (auto a : delta) {
            for (auto b : delta) {
                for (auto c : delta) {
                    projection::Rotation rot(0. + static_cast<double>(b),
                                             -90. + static_cast<double>(a),
                                             static_cast<double>(c));
                    EXPECT(rot.rotated() == (a % 360 != 0 || (b - c) % 360 != 0));

                    EXPECT(points_equal(p, rot.inv(rot.fwd(p))));
                    EXPECT(points_equal(p, rot.fwd(rot.inv(p))));
                }
            }
        }
    }


#if 0
    SECTION("projection type: rotation") {
        const int Ni = 12;
        const int Nj = 3;

        projection::Rotation rot(182., -46.7, 0.);
        const PointLonLat ref[]{
            {-178., -46.7},
            {-178., -16.7},
            {-178., 13.3},
            {-178., 43.3},
            {-178., 73.3},
            {2., 76.7},
            {2., 46.7},
            {-178., -46.7},
            {-162.62343, -19.46929},
            {-152.02366, 8.65459},
            {-139.57464, 36.43683},
            {-113.10894, 61.43199},
            {-39.88245, 68.00825},
            {2., 46.7},
            {-178., -46.7},
            {-148.83443, -27.31067},
            {-129.26346, -3.837},
            {-110.79116, 20.05422},
            {-85.87917, 41.36507},
            {-44.42496, 53.29508},
            {2., 46.7},
            {-178., -46.7},
            {-137.90794, -39.07002},
            {-109.60146, -21.33906},
            {-88., 0.},
            {-66.39854, 21.33906},
            {-38.09206, 39.07002},
            {2., 46.7},
            {-178., -46.7},
            {-131.57504, -53.29508},
            {-90.12083, -41.36507},
            {-65.20884, -20.05422},
            {-46.73654, 3.837},
            {-27.16557, 27.31067},
            {2., 46.7},
            {-178., -46.7},
            {-136.11755, -68.00825},
            {-62.89106, -61.43199},
            {-36.42536, -36.43683},
            {-23.97634, -8.65459},
            {-13.37657, 19.46929},
            {2., 46.7},
            {-178., -46.7},
            {-178., -76.7},
            {2., -73.3},
            {2., -43.3},
            {2., -13.3},
            {2., 16.7},
            {2., 46.7},
            {-178., -46.7},
            {140.11755, -68.00825},
            {66.89106, -61.43199},
            {40.42536, -36.43683},
            {27.97634, -8.65459},
            {17.37657, 19.46929},
            {2., 46.7},
            {-178., -46.7},
            {135.57504, -53.29508},
            {94.12083, -41.36507},
            {69.20884, -20.05422},
            {50.73654, 3.837},
            {31.16557, 27.31067},
            {2., 46.7},
            {-178., -46.7},
            {141.90794, -39.07002},
            {113.60146, -21.33906},
            {92., 0.},
            {70.39854, 21.33906},
            {42.09206, 39.07002},
            {2., 46.7},
            {-178., -46.7},
            {152.83443, -27.31067},
            {133.26346, -3.837},
            {114.79116, 20.05422},
            {89.87917, 41.36507},
            {48.42496, 53.29508},
            {2., 46.7},
            {-178., -46.7},
            {166.62343, -19.46929},
            {156.02366, 8.65459},
            {143.57464, 36.43683},
            {117.10894, 61.43199},
            {43.88245, 68.00825},
            {2., 46.7},
        };

        for (int i = 0, k = 0; i < Ni; i++) {
            for (int j = 0; j < 2 * Nj + 1; j++, k++) {
                PointLonLat a(static_cast<double>(i) * 360. / static_cast<double>(Ni),
                              static_cast<double>(j - Nj) * 90. / static_cast<double>(Nj));
                auto b = rot.fwd(a);
                auto c = rot.inv(b);

                EXPECT(points_equal(b, ref[k]));
                EXPECT(points_equal(a, c));
            }
        }
    }
#endif


    SECTION("projection type: rotation") {
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
            EXPECT(points_equal(b, test.b));

            auto a = test.rotation.inv(b);
            EXPECT(points_equal(a, test.a));
        }
    }


    SECTION("projection type: proj") {
        if (ProjectionFactory::instance().exists("proj")) {
            std::cout.precision(14);

            PointLonLat a{12., 55.};

            struct {
                const Point b;
                const std::string target;
            } tests[] = {
                {Point2{691875.632137542, 6098907.825129169}, "+proj=utm +zone=32 +datum=WGS84"},
                {Point2{691875.632137542, 6098907.825129169}, "EPSG:32632"},
                {a, "EPSG:4326"},
                {a, "EPSG:4979"},
                {Point3{3586469.6567764, 762327.65877826, 5201383.5232023}, "EPSG:4978"},
                {Point3{3574529.7050235, 759789.74368715, 5219005.2599833}, "+proj=cart +R=6371229."},
                {Point3{3574399.5431832, 759762.07693392, 5218815.216709}, "+proj=cart +ellps=sphere"},
                {a, "+proj=latlon +ellps=sphere"},
            };

            for (const auto& test : tests) {
                P projection(ProjectionFactory::instance().get("proj").create(
                    spec::MappedConfiguration{{{"source", "EPSG:4326"}, {"target", test.target}}}));

#if 0
        std::cout << "ellipsoid: '" << PROJ::ellipsoid(projection.target())
                  << std::endl;
#endif

                auto b = projection->fwd(a);
                auto c = projection->inv(b);

                std::cout << "-> a:" << a << " -> fwd(a):" << b << " -> inv(fwd(a)):" << c << std::endl;

                EXPECT(points_equal(b, test.b));
                EXPECT(points_equal(c, a));

                P reverse(ProjectionFactory::instance().get("proj").create(
                    spec::MappedConfiguration({{"source", test.target}, {"target", "EPSG:4326"}})));

                auto d = reverse->fwd(test.b);
                auto e = reverse->inv(d);

                std::cout << "-> b:" << test.b << " -> fwd(b):" << d << " -> inv(fwd(b)):" << e << std::endl;

                EXPECT(points_equal(d, a));
                EXPECT(points_equal(e, test.b));
            }
        }
    }


    SECTION("projection type: mercator") {
        PointLonLat first{262.036, 14.7365};

        {
            projection::Mercator projection(0., 14., new figure::Sphere(6371229.), first);

            auto p = projection.inv({0, 0});
            Log::info() << p << std::endl;

            auto q = projection.fwd(p);
            Log::info() << q << std::endl;
        }

        {
            projection::Mercator projection(-180., 0., new figure::Sphere(1.), {0, 0});

            auto q = projection.fwd({-75, 35});
            Log::info() << q << std::endl;

            auto p = projection.inv(q);
            Log::info() << p << std::endl;
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
