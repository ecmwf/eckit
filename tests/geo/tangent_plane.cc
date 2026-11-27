// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <memory>

#include "eckit/geo/Figure.h"
#include "eckit/geo/TangentPlane.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal((a), (b), (eps)))


namespace eckit::geo::test {


// unit vectors from a few trigonometric operations (orthonormal eps, smaller than PointXYZ::EPS)
constexpr double EPS = 1e-12;


void expect_orthonormal(const TangentPlane& plane, const PointXYZ& up) {
    EXPECT_APPROX(dot(plane.east(), plane.east()), 1., EPS);
    EXPECT_APPROX(dot(plane.north(), plane.north()), 1., EPS);
    EXPECT_APPROX(dot(plane.east(), plane.north()), 0., EPS);
    EXPECT_APPROX(dot(plane.east(), up), 0., EPS);
    EXPECT_APPROX(dot(plane.north(), up), 0., EPS);

    // right-handed (east, north, up)
    EXPECT_APPROX(dot(cross(plane.east(), plane.north()), up), 1., EPS);
}


CASE("sphere: axes") {
    const figure::Sphere sphere(1.);
    const projection::LonLatToXYZ to_xyz(1.);

    SECTION("at the origin of longitude/latitude") {
        const PointLonLat geodetic{0., 0.};
        const auto origin = to_xyz.fwd(geodetic);

        for (const auto& plane : {TangentPlane(origin), TangentPlane(sphere, geodetic)}) {
            EXPECT(points_equal(plane.origin(), origin));
            EXPECT(points_equal(plane.east(), {0., 1., 0.}));
            EXPECT(points_equal(plane.north(), {0., 0., 1.}));
        }
    }

    SECTION("orthonormal, both constructors agree") {
        for (const auto& geodetic : {PointLonLat{10., 20.}, PointLonLat{-170., -45.}, PointLonLat{90., 89.}}) {
            const auto origin = to_xyz.fwd(geodetic);
            const TangentPlane a(origin);
            const TangentPlane b(sphere, geodetic);

            expect_orthonormal(a, origin);
            expect_orthonormal(b, origin);
            EXPECT(points_equal(a.origin(), b.origin()));
            EXPECT(points_equal(a.east(), b.east()));
            EXPECT(points_equal(a.north(), b.north()));
        }
    }

    SECTION("at the poles, east is the Y axis") {
        for (double lat : {90., -90.}) {
            const PointLonLat geodetic{0., lat};
            const auto origin = to_xyz.fwd(geodetic);

            for (const auto& plane : {TangentPlane(origin), TangentPlane(sphere, geodetic)}) {
                expect_orthonormal(plane, {0., 0., lat > 0. ? 1. : -1.});
                EXPECT(points_equal(plane.east(), {0., 1., 0.}));
                EXPECT(points_equal(plane.north(), {lat > 0. ? -1. : 1., 0., 0.}));
            }
        }
    }
}


CASE("sphere: offsets of nearby points") {
    constexpr double R = 2.;
    constexpr double h = 1e-5;  // [degree]
    const auto dh      = h * util::DEGREE_TO_RADIAN;

    const figure::Sphere sphere(R);
    const projection::LonLatToXYZ to_xyz(R);

    for (const auto& geodetic : {PointLonLat{0., 0.}, PointLonLat{30., 60.}, PointLonLat{-120., -30.}}) {
        const TangentPlane plane(sphere, geodetic);
        const auto lon = geodetic.lon();
        const auto lat = geodetic.lat();

        const auto o = plane(plane.origin());
        EXPECT_APPROX(o.X(), 0., PointXYZ::EPS);
        EXPECT_APPROX(o.Y(), 0., PointXYZ::EPS);

        const auto e = plane(to_xyz.fwd({lon + h, lat}));
        EXPECT_APPROX(e.X() / (R * std::cos(lat * util::DEGREE_TO_RADIAN) * dh), 1., 1e-8);
        EXPECT_APPROX(e.Y() / (R * dh), 0., 1e-4);

        const auto n = plane(to_xyz.fwd({lon, lat + h}));
        EXPECT_APPROX(n.X() / (R * dh), 0., 1e-8);
        EXPECT_APPROX(n.Y() / (R * dh), 1., 1e-8);
    }
}


CASE("oblate spheroid: normal to the surface, offsets by the radii of curvature") {
    std::unique_ptr<Figure> wgs84(FigureFactory::build(spec::Custom{{"figure", "wgs84"}}));

    const projection::LonLatToXYZ to_xyz(wgs84->a(), wgs84->b());

    constexpr double h = 1e-5;  // [degree]
    const auto dh      = h * util::DEGREE_TO_RADIAN;

    for (const auto& geodetic : {PointLonLat{0., 0.}, PointLonLat{30., 45.}, PointLonLat{-120., -75.}}) {
        const auto lon = geodetic.lon();
        const auto lat = geodetic.lat();
        const auto phi = lat * util::DEGREE_TO_RADIAN;
        const auto lam = lon * util::DEGREE_TO_RADIAN;

        const TangentPlane plane(*wgs84, geodetic);
        EXPECT(points_equal(plane.origin(), to_xyz.fwd(geodetic)));

        // the surface normal (geodetic), not the radial direction
        const PointXYZ normal{std::cos(phi) * std::cos(lam), std::cos(phi) * std::sin(lam), std::sin(phi)};
        expect_orthonormal(plane, normal);

        const auto e = plane(to_xyz.fwd({lon + h, lat}));
        EXPECT_APPROX(e.X() / (wgs84->prime_vertical_radius(lat) * std::cos(phi) * dh), 1., 1e-8);

        const auto n = plane(to_xyz.fwd({lon, lat + h}));
        EXPECT_APPROX(n.X() / (wgs84->meridional_radius(lat) * dh), 0., 1e-8);
        EXPECT_APPROX(n.Y() / (wgs84->meridional_radius(lat) * dh), 1., 1e-8);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
