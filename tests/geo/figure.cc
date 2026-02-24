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


#include <cmath>
#include <memory>

#include "eckit/geo/Figure.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/figure/SphereT.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


struct F : std::unique_ptr<Figure> {
    explicit F(Figure* ptr) : unique_ptr(ptr) { ASSERT(unique_ptr::operator bool()); }
};


struct DatumTwoUnits {
    static constexpr double radius = 2.;

    static constexpr double a = radius;
    static constexpr double b = radius;
};


CASE("Sphere") {
    F f1(FigureFactory::build(spec::Custom{{"R", 1.}}));
    F f2(FigureFactory::build(spec::Custom{{"a", 1.}, {"b", 1.}}));
    F f3(new figure::Sphere(1.));

    EXPECT_THROWS_AS(figure::Sphere(-1.), BadValue);

    EXPECT(*f1 == *f2);
    EXPECT(*f1 == *f3);

    auto e = f1->eccentricity();
    EXPECT(types::is_approximately_equal(e, 0.));

    EXPECT(f1->spec_str() == R"({"r":1})");
}


CASE("Oblate spheroid") {
    F f1(FigureFactory::build(spec::Custom{{"b", 0.5}, {"a", 1.}}));
    F f2(new figure::OblateSpheroid(1., 0.5));

    EXPECT_THROWS_AS(figure::OblateSpheroid(0.5, 1.), BadValue);  // prolate spheroid
    EXPECT_THROWS_AS(figure::OblateSpheroid(1., -1.), BadValue);

    EXPECT(*f1 == *f2);

    auto e = f1->eccentricity();
    EXPECT(types::is_strictly_greater(e, 0.));

    EXPECT(f1->spec_str() == R"({"a":1,"b":0.5})");
}


CASE("Unit Sphere") {
    figure::UnitSphere s1;

    const auto R = s1.radius();
    const auto L = R * std::sqrt(2) / 2.;

    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai


    SECTION("radius") {
        EXPECT(s1.radius() == 1.);
    }


    SECTION("north pole") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({0., 90.}), PointXYZ{0, 0, R}));
    }


    SECTION("south pole") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({0., -90.}), PointXYZ{0, 0, -R}));
    }


    SECTION("distances") {
        // Same points with added shifts
        auto P1b = PointLonLat::make(288.4, -33.);    // Valparaíso + longitude shift
        auto P2b = PointLonLat::make(301.8, 148.6);   // Shanghai + latitude/longitude shift
        auto P2c = PointLonLat::make(-58.2, -211.4);  // Shanghai + latitude/longitude shift

        auto d0 = s1.distance(P1, P2);
        auto d1 = s1.distance(P1b, P2);
        auto d2 = s1.distance(P1, P2b);
        auto d3 = s1.distance(P1, P2c);

        EXPECT(types::is_approximately_equal(d0, d1));
        EXPECT(types::is_approximately_equal(d0, d2));
        EXPECT(types::is_approximately_equal(d0, d3));
    }


    SECTION("area globe") {
        EXPECT(s1.area() == 4. * M_PI * R * R);
    }


    SECTION("area hemispheres") {
        auto area_hemisphere_north = s1._area({90., -180., 0., 180.});
        auto area_hemisphere_south = s1._area({0., -180., -90., 180.});

        EXPECT(area_hemisphere_north == 0.5 * s1.area());
        EXPECT(area_hemisphere_north == area_hemisphere_south);
    }


    SECTION("lon 0 (quadrant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({0., 0.}), PointXYZ{R, 0, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-360., 0.}), PointXYZ{R, 0, 0}));
    }


    SECTION("lon 90 (quadrant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({90., 0.}), PointXYZ{0, R, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-270., 0.}), PointXYZ{0, R, 0}));
    }


    SECTION("lon 180 (quadrant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({180., 0.}), PointXYZ{-R, 0, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-180., 0.}), PointXYZ{-R, 0, 0}));
    }


    SECTION("lon 270 (quadrant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({270., 0.}), PointXYZ{0, -R, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-90., 0.}), PointXYZ{0, -R, 0}));
    }


    SECTION("lon 45 (octant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({45., 0.}), PointXYZ{L, L, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-315., 0.}), PointXYZ{L, L, 0}));
    }


    SECTION("lon 135 (octant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({135., 0.}), PointXYZ{-L, L, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-225., 0.}), PointXYZ{-L, L, 0}));
    }


    SECTION("lon 225 (octant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({225., 0.}), PointXYZ{-L, -L, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-135., 0.}), PointXYZ{-L, -L, 0}));
    }


    SECTION("lon 315 (octant)") {
        EXPECT(points_equal(s1._convertSphericalToCartesian({315., 0.}), PointXYZ{L, -L, 0}));
        EXPECT(points_equal(s1._convertSphericalToCartesian({-45., 0.}), PointXYZ{L, -L, 0}));
    }


    SECTION("lat 100") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(0., 100.)), BadValue);

        auto p = s1._convertSphericalToCartesian(PointLonLat::make(0., 100.), 0.);
        auto q = s1._convertSphericalToCartesian(PointLonLat::make(180., 80.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X(), q.X()));
        EXPECT(types::is_approximately_equal(p.Y(), q.Y()));
        EXPECT(types::is_approximately_equal(p.Z(), q.Z()));
    }


    SECTION("lat 290") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(15., 290.)), BadValue);

        auto p = s1._convertSphericalToCartesian(PointLonLat::make(15., 290.), 0.);
        auto q = s1._convertSphericalToCartesian(PointLonLat::make(15., -70.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X(), q.X()));
        EXPECT(types::is_approximately_equal(p.Y(), q.Y()));
        EXPECT(types::is_approximately_equal(p.Z(), q.Z()));
    }


    SECTION("lat -120") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(45., -120.)), BadValue);

        auto p = s1._convertSphericalToCartesian(PointLonLat::make(45., -120.), 0.);
        auto q = s1._convertSphericalToCartesian(PointLonLat::make(225., -60.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X(), q.X()));
        EXPECT(types::is_approximately_equal(p.Y(), q.Y()));
        EXPECT(types::is_approximately_equal(p.Z(), q.Z()));
    }
}


CASE("Two-unit Sphere") {
    figure::UnitSphere s1;
    figure::SphereT<DatumTwoUnits> s2;

    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai


    SECTION("radius") {
        EXPECT(s2.radius() == 2.);
    }


    SECTION("distances") {
        auto distance_1 = s1.distance(P1, P2);
        auto distance_2 = s2.distance(P1, P2);
        EXPECT(2. * distance_1 == distance_2);
    }


    SECTION("area") {
        auto global_1 = s1.area();
        auto global_2 = s2.area();
        EXPECT(4. * global_1 == global_2);

        area::BoundingBox bbox({P2.lat(), P1.lon(), P1.lat(), P2.lon()});

        auto local_1 = s1._area(bbox);
        auto local_2 = s2._area(bbox);
        EXPECT(4. * local_1 == local_2);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
