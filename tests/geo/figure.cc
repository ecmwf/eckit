// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
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

    static constexpr bool default_figure = false;
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

    EXPECT(f1->spec_str() == R"({"figure":{"r":1}})");
}


CASE("Oblate spheroid") {
    F f1(FigureFactory::build(spec::Custom{{"b", 0.5}, {"a", 1.}}));
    F f2(new figure::OblateSpheroid(1., 0.5));

    EXPECT_THROWS_AS(figure::OblateSpheroid(0.5, 1.), BadValue);  // prolate spheroid
    EXPECT_THROWS_AS(figure::OblateSpheroid(1., -1.), BadValue);

    EXPECT(*f1 == *f2);

    auto e = f1->eccentricity();
    EXPECT(types::is_strictly_greater(e, 0.));

    EXPECT(f1->spec_str() == R"({"figure":{"a":1,"b":0.5}})");
}


CASE("Figure: spec, and as part of a projection's") {
    struct test_t {
        std::string figure;
        std::string spec;
        std::string projection_spec;
    };

    for (const auto& test : std::vector<test_t>{
             {"{figure: earth}", R"({"figure":"earth"})", "{}"},
             {"{figure: grib1}", R"({"figure":"grib1"})", "{}"},
             {"{figure: wgs84}", R"({"figure":"wgs84"})", R"({"figure":"wgs84"})"},
             {"{R: 1}", R"({"figure":{"r":1}})", R"({"figure":{"r":1}})"},
         }) {
        F figure(FigureFactory::make_from_string(test.figure));
        EXPECT_EQUAL(figure->spec_str(), test.spec);

        spec::Custom projection_spec;
        figure->fill_projection_spec(projection_spec);
        EXPECT_EQUAL(projection_spec.str(), test.projection_spec);
    }
}


CASE("Inline figure") {
    // figure described inline by a (JSON/YAML) string, as opposed to by name or by a sub-spec
    F f1(FigureFactory::build(spec::Custom{{"figure", R"({"R":6371229})"}}));
    F f2(FigureFactory::build(spec::Custom{{"figure", " {R: 6371229.000000}"}}));
    F f3(FigureFactory::build(spec::Custom{{"figure", "earth"}}));

    EXPECT(*f1 == *f2);
    EXPECT(*f1 == *f3);

    // figures equivalent to a default figure are default figures
    EXPECT(f1->is_default());
    EXPECT(F(FigureFactory::build(spec::Custom{{"figure", R"({"R":6367470})"}}))->is_default());
    EXPECT(!F(FigureFactory::build(spec::Custom{{"figure", R"({"R":6371200})"}}))->is_default());

    F f4(FigureFactory::build(spec::Custom{{"figure", R"({"a":6378140,"b":6356755})"}}));
    F f5(new figure::OblateSpheroid(6378140., 6356755.));

    EXPECT(*f4 == *f5);
    EXPECT(!f4->is_default());

    F f6(FigureFactory::make_from_string(R"({"figure":"{\"a\":1,\"b\":0.5}"})"));
    F f7(new figure::OblateSpheroid(1., 0.5));

    EXPECT(*f6 == *f7);

    EXPECT_THROWS_AS(F(FigureFactory::build(spec::Custom{{"figure", "not_a_figure"}})), BadParameter);
}


CASE("Figure::proj_str") {
    struct test_t {
        spec::Custom spec;
        std::string proj_str;
    } tests[] = {
        {spec::Custom{{"figure", "wgs84"}}, "+ellps=WGS84"},
        {spec::Custom{{"figure", "grs80"}}, "+ellps=GRS80"},
        {spec::Custom{{"figure", "earth"}}, "+R=6371229"},
        {spec::Custom{{"figure", "wgs84_sphere"}}, "+R=6371200"},
        {spec::Custom{{"figure", "unit-sphere"}}, "+R=1"},
        {spec::Custom{{"R", 6378206.4}}, "+R=6378206.4"},
        {spec::Custom{{"r", 6378206.4}}, "+R=6378206.4"},
        {spec::Custom{{"radius", 6378206.4}}, "+R=6378206.4"},
        {spec::Custom{{"a", 6378206.4}, {"b", 6356583.8}}, "+a=6378206.4 +b=6356583.8"},

        {spec::Custom{{"a", 6378137.}, {"b", 6356752.314245}}, "+ellps=WGS84"},
        {spec::Custom{{"a", 6378137.}, {"b", 6356752.31414}}, "+ellps=GRS80"},
        {spec::Custom{{"figure", R"({"a":6378137,"b":6356752.314245})"}}, "+ellps=WGS84"},
        {spec::Custom{{"semi_major_axis", 6378137.}, {"semi_minor_axis", 6356752.314245}}, "+ellps=WGS84"},

        {spec::Custom{{"figure", "iau1965"}}, "+a=6378160 +b=6356775"},
        {spec::Custom{{"semi_major_axis", 6378160.}, {"semi_minor_axis", 6356775.}}, "+a=6378160 +b=6356775"},
        {spec::Custom{{"a", 6371229.}, {"b", 6371229.}}, "+R=6371229"},
    };

    for (const auto& test : tests) {
        EXPECT_EQUAL(F(FigureFactory::build(test.spec))->proj_str(), test.proj_str);
    }
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
