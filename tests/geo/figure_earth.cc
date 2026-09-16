// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


struct F : std::shared_ptr<Figure> {
    explicit F(Figure* ptr) : shared_ptr(ptr) { ASSERT(operator bool()); }
};


CASE("Earth") {
    F f1(FigureFactory::build(spec::Custom{{"r", 6371229.}}));
    F f2(new figure::Earth);

    EXPECT(*f1 == *f2);
    EXPECT(f1->spec_str() == R"({"figure":"earth"})");
    EXPECT(types::is_approximately_equal(f1->R(), 6371229., 1e-8));

    F f4(FigureFactory::build(spec::Custom{{"figure", "wgs84"}}));

    EXPECT(f4->spec_str() == R"({"figure":"wgs84"})");
    EXPECT(types::is_approximately_equal(1. / f4->flattening(), 298.257223563, 1e-8));
    EXPECT_THROWS_AS(f4->R(), BadValue);
}


CASE("Named spheres") {
    struct test_t {
        const std::string name;
        const double radius;
    };

    for (const auto& test : std::vector<test_t>{
             {"earth", figure::DatumIFS::radius},
             {"grib1", figure::DatumGRIB1::radius},
             {"wgs84_sphere", figure::DatumWgs84Sphere::radius},
         }) {
        SECTION(test.name) {
            F f1(FigureFactory::build(spec::Custom{{"figure", test.name}}));
            F f2(FigureFactory::build(spec::Custom{{"r", test.radius}}));

            EXPECT(*f1 == *f2);
            EXPECT(f1->spherical());
            EXPECT(types::is_approximately_equal(f1->R(), test.radius));
            EXPECT(types::is_approximately_equal(f1->a(), test.radius));
            EXPECT(types::is_approximately_equal(f1->b(), test.radius));
            EXPECT(types::is_approximately_equal(f1->eccentricity(), 0.));

            // a named sphere round-trips by name
            EXPECT(f1->spec_str() == R"({"figure":")" + test.name + R"("})");
        }
    }
}


CASE("Named oblate spheroids") {
    struct test_t {
        const std::string name;
        const double a;
        const double b;
        const double inverse_flattening;
    };

    for (const auto& test : std::vector<test_t>{
             {"grs80", figure::DatumGrs80::a, figure::DatumGrs80::b, 298.257222101},
             {"wgs84", figure::DatumWgs84::a, figure::DatumWgs84::b, 298.257223563},
             {"iau1965", figure::DatumIau1965::a, figure::DatumIau1965::b, 298.253916296},
         }) {
        SECTION(test.name) {
            F f1(FigureFactory::build(spec::Custom{{"figure", test.name}}));
            F f2(FigureFactory::build(spec::Custom{{"a", test.a}, {"b", test.b}}));

            EXPECT(*f1 == *f2);
            EXPECT(!f1->spherical());
            EXPECT(types::is_approximately_equal(f1->a(), test.a));
            EXPECT(types::is_approximately_equal(f1->b(), test.b));
            EXPECT(types::is_approximately_equal(1. / f1->flattening(), test.inverse_flattening, 1e-8));
            EXPECT_THROWS_AS(f1->R(), BadValue);

            // a named spheroid round-trips by name
            EXPECT(f1->spec_str() == R"({"figure":")" + test.name + R"("})");
        }
    }
}


CASE("Distinct figures") {
    // all tabulated figures are distinguishable from one another
    std::vector<F> figures;
    for (const auto* name : {"earth", "grib1", "grs80", "iau1965", "wgs84", "wgs84_sphere"}) {
        figures.emplace_back(FigureFactory::build(spec::Custom{{"figure", name}}));
    }

    for (size_t i = 0; i < figures.size(); ++i) {
        for (size_t j = i + 1; j < figures.size(); ++j) {
            EXPECT(*figures[i] != *figures[j]);
        }
    }

    // GRS80 and WGS84 only differ in the semi-minor axis, by less than a millimetre
    EXPECT(types::is_approximately_equal(figure::DatumGrs80::a, figure::DatumWgs84::a));
    EXPECT(!types::is_approximately_equal(figure::DatumGrs80::b, figure::DatumWgs84::b));
    EXPECT(std::abs(figure::DatumGrs80::b - figure::DatumWgs84::b) < 1e-3);
}


CASE("Area") {
    struct test_t {
        test_t(const std::string& _figure, double _area) :
            figure(FigureFactory::build(spec::Custom{{"figure", _figure}})), area(_area) {}

        const F figure;
        const double area;
    };

    for (const auto& test : std::vector<test_t>{
             {"earth", 510101140.},  //[km^2]
             {"wgs84", 510065621.},
             {"grs80", 510065621.},
             {"iau1965", 510069287.},
             {"grib1", 509499402.},
             {"wgs84_sphere", 510096496.},
         }) {
        EXPECT(types::is_approximately_equal(test.figure->area() * 1e-6 /*[km^2]*/, test.area, 1.));
    }
}


CASE("projection::LonLatToXYZ") {
    const auto R = figure::EARTH.R();
    const auto L = R * std::sqrt(2) / 2.;

    projection::LonLatToXYZ to_xyz(new figure::Earth);
    projection::LonLatToXYZ to_xyz_default;


    SECTION("lon 0 (quadrant)") {
        PointXYZ p{R, 0, 0};
        EXPECT(points_equal(to_xyz.fwd({0., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-360., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({0., 0.}), p));
    }


    SECTION("lon 90 (quadrant)") {
        PointXYZ p{0, R, 0};
        EXPECT(points_equal(to_xyz.fwd({90., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-270., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({90., 0.}), p));
    }


    SECTION("lon 180 (quadrant)") {
        PointXYZ p{-R, 0, 0};
        EXPECT(points_equal(to_xyz.fwd({180., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-180., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({180., 0.}), p));
    }


    SECTION("lon 270 (quadrant)") {
        PointXYZ p{0, -R, 0};
        EXPECT(points_equal(to_xyz.fwd({270., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-90., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({270., 0.}), p));
    }


    SECTION("lon 45 (octant)") {
        PointXYZ p{L, L, 0};
        EXPECT(points_equal(to_xyz.fwd({45., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-315., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({45., 0.}), p));
    }


    SECTION("lon 135 (octant)") {
        PointXYZ p{-L, L, 0};
        EXPECT(points_equal(to_xyz.fwd({135., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-225., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({135., 0.}), p));
    }


    SECTION("lon 225 (octant)") {
        PointXYZ p{-L, -L, 0};
        EXPECT(points_equal(to_xyz.fwd({225., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-135., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({225., 0.}), p));
    }


    SECTION("lon 315 (octant)") {
        PointXYZ p{L, -L, 0};
        EXPECT(points_equal(to_xyz.fwd({315., 0.}), p));
        EXPECT(points_equal(to_xyz.fwd({-45., 0.}), p));
        EXPECT(points_equal(to_xyz_default.fwd({315., 0.}), p));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
