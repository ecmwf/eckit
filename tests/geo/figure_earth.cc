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
    EXPECT(f1->spec_str() == R"({"r":6371229})");
    EXPECT(types::is_approximately_equal(f1->R(), 6371229., 1e-8));

    F f4(FigureFactory::build(spec::Custom{{"figure", "wgs84"}}));

    EXPECT(f4->spec_str() == R"({"figure":"wgs84"})");
    EXPECT(types::is_approximately_equal(1. / f4->flattening(), 298.257223563, 1e-8));
    EXPECT_THROWS_AS(f4->R(), BadValue);
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
