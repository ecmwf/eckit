// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/LonLatHToXYZ.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/projection/ToFromXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("projection: llh-to-xyz") {
    struct P : std::unique_ptr<Projection> {
        explicit P(Projection* ptr) : unique_ptr(ptr) { ASSERT(unique_ptr::operator bool()); }
    };

    struct test_t {
        PointLonLatH a;
        PointXYZ b;
    };

    P sphere(Factory<Projection>::instance().get("llh-to-xyz").create(spec::Custom{{"R", 1.}}));
    P sphere_scaled(Factory<Projection>::instance()
                        .get("llh-to-xyz")
                        .create(spec::Custom{{"R", 1.}, {"height_scale_factor", 25.}}));
    P spheroid(Factory<Projection>::instance().get("llh-to-xyz").create(spec::Custom{{"a", 1.}, {"b", 0.5}}));

    EXPECT(*sphere == projection::LonLatHToXYZ(new figure::Sphere(1.)));
    EXPECT(*sphere_scaled == projection::LonLatHToXYZ(new figure::Sphere(1.), 25.));
    EXPECT(*spheroid == projection::LonLatHToXYZ(new figure::OblateSpheroid(1., 0.5)));
    EXPECT_NOT(*sphere == *sphere_scaled);

    EXPECT_THROWS_AS(projection::LonLatHToXYZ(new figure::Sphere(1.), -1.), exception::ProjectionError);


    SECTION("spec") {
        EXPECT(sphere->spec_str() == R"({"figure":{"r":1},"type":"llh-to-xyz"})");
        EXPECT(sphere_scaled->spec_str() == R"({"figure":{"r":1},"height_scale_factor":25,"type":"llh-to-xyz"})");
        EXPECT(spheroid->spec_str() == R"({"figure":{"a":1,"b":0.5},"type":"llh-to-xyz"})");
    }


    SECTION("point coordinates") {
        using names = std::vector<std::string>;
        EXPECT((sphere->source_point_coordinates() == names{"longitude", "latitude", "height"}));
        EXPECT((sphere->target_point_coordinates() == names{"x", "y", "z"}));
    }


    SECTION("sphere (llh -> xyz, xyz -> llh)") {
        for (const auto& test : std::vector<test_t>{
                 {{0, 0, 1}, {2, 0, 0}},       //
                 {{90, 0, 0.5}, {0, 1.5, 0}},  //
                 {{0, 90, 1}, {0, 0, 2}},      //
                 {{180, 0, -0.5}, {-0.5, 0, 0}},
             }) {
            EXPECT(points_equal(std::get<PointXYZ>(sphere->fwd(test.a)), test.b));
            EXPECT(points_equal(std::get<PointLonLatH>(sphere->inv(test.b)), test.a));
        }

        // height is scaled
        EXPECT(points_equal(std::get<PointXYZ>(sphere_scaled->fwd(PointLonLatH{0, 0, 0.1})), PointXYZ{3.5, 0, 0}));
        EXPECT(points_equal(std::get<PointLonLatH>(sphere_scaled->inv(PointXYZ{3.5, 0, 0})), PointLonLatH{0, 0, 0.1}));
    }


    SECTION("height 0 (or height scale factor 0) as ll-to-xyz") {
        const projection::LonLatToXYZ ll_sphere(new figure::Sphere(1.));
        const projection::LonLatToXYZ ll_spheroid(new figure::OblateSpheroid(1., 0.5));
        const projection::LonLatHToXYZ unscaled(new figure::OblateSpheroid(1., 0.5), 0.);

        for (const auto& p : {PointLonLat{0, 0}, PointLonLat{30, 40}, PointLonLat{-120, -60}, PointLonLat{42, 90}}) {
            EXPECT(points_equal(std::get<PointXYZ>(sphere->fwd(PointLonLatH{p, 0})), ll_sphere.fwd(p)));
            EXPECT(points_equal(std::get<PointXYZ>(spheroid->fwd(PointLonLatH{p, 0})), ll_spheroid.fwd(p)));
            EXPECT(points_equal(unscaled.fwd(PointLonLatH{p, 100.}), ll_spheroid.fwd(p)));
        }
    }


    SECTION("spheroid, height along the normal") {
        // geodetic height: the point moves by h along the unit normal, the unit sphere direction of (lon, lat)
        const projection::LonLatToXYZ normal(new figure::Sphere(1.));

        for (const auto& p : {PointLonLat{0, 0}, PointLonLat{30, 40}, PointLonLat{-120, -60}}) {
            const double h = 0.25;
            const auto q0  = std::get<PointXYZ>(spheroid->fwd(PointLonLatH{p, 0}));
            const auto qh  = std::get<PointXYZ>(spheroid->fwd(PointLonLatH{p, h}));
            const auto n   = normal.fwd(p);
            EXPECT(points_equal(qh, PointXYZ{q0.X() + h * n.X(), q0.Y() + h * n.Y(), q0.Z() + h * n.Z()}));
        }

        // not invertible on a spheroid, nor without a height scale factor
        EXPECT_THROWS(spheroid->inv(PointXYZ{1, 0, 0}));
        EXPECT_THROWS(projection::LonLatHToXYZ(new figure::Sphere(1.), 0.).inv(PointXYZ{1, 0, 0}));
    }


    SECTION("many points") {
        const std::vector<double> lon{0, 90, 30};
        const std::vector<double> lat{0, 0, 40};
        const std::vector<double> height{1, 0.5, 0.25};

        const auto xyz = sphere_scaled->fwd(lon, lat, height);
        ASSERT(xyz.size() == 3);

        for (size_t i = 0; i < lon.size(); ++i) {
            const auto q = std::get<PointXYZ>(sphere_scaled->fwd(PointLonLatH{lon[i], lat[i], height[i]}));
            EXPECT(points_equal(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, q));
        }

        // (lon, lat) only is not enough
        EXPECT_THROWS(sphere_scaled->fwd(lon, lat));
    }
}


CASE("ToFromXYZ: to_lonlat, to_lonlath") {
    using projection::ToFromXYZ;

    constexpr double R = 2.;

    for (const auto& p : {PointLonLat{0, 0}, PointLonLat{30, 40}, PointLonLat{-120, -60}, PointLonLat{0, 90}}) {
        for (const double h : {0., 0.5, -1.}) {
            const auto q = ToFromXYZ::to_xyz(R, p, h);

            // the direction, at any distance from the centre
            EXPECT(points_equal(ToFromXYZ::to_lonlat(q), p));

            const auto r = ToFromXYZ::to_lonlath(R, q);
            EXPECT(points_equal(r.lonlat(), p));
            EXPECT(types::is_approximately_equal(r.height(), h, 1e-12));
        }
    }

    EXPECT_THROWS_AS(ToFromXYZ::to_lonlath(R, PointXYZ{0, 0, 0}), exception::ProjectionError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
