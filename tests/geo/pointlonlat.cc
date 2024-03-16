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


#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Point.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("PointLonLat normalise_angle_to_*") {
    SECTION("normalise_angle_to_minimum") {
        struct {
            double angle;
            double lim;
            double ref;
        } tests[]{
            {10., 0., 10.},
            {0., 0., 0.},
            {-10., 0., 350.},
            {720., 0., 0.},
            {100., 90., 100.},
            {-370., 0., 350.},
            {100000., 0., static_cast<double>(100000 % 360)},
            {-100., -180., -100.},
            {360., 0., 0.},
            {100000., 99960., 100000.},
        };

        for (const auto& test : tests) {
            EXPECT(types::is_approximately_equal(test.ref,
                                                 PointLonLat::normalise_angle_to_minimum(test.angle, test.lim),
                                                 PointLonLat::EPS));
        }
    }


    SECTION("normalise_angle_to_maximum") {
        struct {
            double angle;
            double lim;
            double ref;
        } tests[]{
            {350., 360., 350.},
            {360., 360., 360.},
            {361., 360., 1.},
            {-720., 360., 360.},
            {100., 180., 100.},
            {-370., 360., 350.},
            {100000., 360., static_cast<double>(100000 % 360)},
            {-100., -90., -100.},
            {720., 360., 360.},
            {100040., 100080., 100040.},
        };

        for (const auto& test : tests) {
            EXPECT(types::is_approximately_equal(test.ref,
                                                 PointLonLat::normalise_angle_to_maximum(test.angle, test.lim),
                                                 PointLonLat::EPS));
        }
    }
}


CASE("PointLonLat normalisation") {
    constexpr auto da = 1e-3;

    for (double a : {-370., -190., -100., -90., -80., -10., 0., 10., 80., 90., 100., 190., 370.}) {
        EXPECT(types::is_approximately_equal(a, PointLonLat::normalise_angle_to_minimum(a, a), PointLonLat::EPS));
        EXPECT(types::is_approximately_equal(a, PointLonLat::normalise_angle_to_maximum(a, a), PointLonLat::EPS));

        EXPECT(types::is_approximately_equal(a + 360.,
                                             PointLonLat::normalise_angle_to_minimum(a - da, a) + da,
                                             PointLonLat::EPS));
        EXPECT(types::is_approximately_equal(a - 360.,
                                             PointLonLat::normalise_angle_to_maximum(a + da, a) - da,
                                             PointLonLat::EPS));
    }

    PointLonLat p(1, 90.);
    EXPECT_EQUAL(p.lon, 1.);
    EXPECT_EQUAL(p.lat, 90.);

    auto p2 = PointLonLat::make(p.lon, p.lat);
    EXPECT_EQUAL(p2.lon, 0.);
    EXPECT(points_equal(p, p2));

    auto p3 = PointLonLat(50., 90.);
    EXPECT(points_equal(p, p3));

    PointLonLat q(1., -90.);
    EXPECT_EQUAL(q.lon, 1.);
    EXPECT_EQUAL(q.lat, -90.);

    auto q2 = q.antipode();
    EXPECT_EQUAL(q2.lon, 0.);
    EXPECT(points_equal(q2, p));

    auto q3 = q2.antipode();
    EXPECT(points_equal(q3, q));
}


CASE("PointLonLat comparison") {
    auto r(PointLonLat::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    Point a1 = PointLonLat{180., 0.};
    Point a2 = PointLonLat{-180., 0.};
    EXPECT(points_equal(a1, a2));

    Point b1 = PointLonLat{0., -90.};
    Point b2 = PointLonLat::make(1., 270.);
    EXPECT(points_equal(b1, b2));

    Point c1 = PointLonLat{300, -30};
    Point c2 = PointLonLat{-59.99999999999996, -30.000000000000018};
    EXPECT(points_equal(c1, c2));

    Point d1 = PointLonLat{-178., -46.7};
    Point d2 = PointLonLat{-178.00000000000003, -46.7};
    EXPECT(points_equal(d1, d2));

    Point e1 = PointLonLat(0., 90.);
    Point e2 = PointLonLat(180., 90.);
    EXPECT(points_equal(e1, e2));

    Point f1 = PointLonLat(-0., -90.);
    Point f2 = PointLonLat(-180., -90.);
    EXPECT(points_equal(f1, f2));
}


CASE("PointLonLat normalise angles") {
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(360., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(374., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(374., -90.));
    EXPECT(374. == PointLonLat::normalise_angle_to_minimum(374., 90.));

    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-346., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-346., -90.));
    EXPECT(374. == PointLonLat::normalise_angle_to_minimum(-346., 90.));
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(360. * 1e12, 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(360. * 1e12 + 14, 0.));
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(-360. * 1e12, 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-360. * 1e12 + 14, 0.));
}


CASE("PointLonLat canonicalise on sphere") {
    const PointLonLat p1(108., 32.);

    // Worse coordinates for the same point:
    const auto p2 = PointLonLat::make(-252., 32.);
    const auto p3 = PointLonLat::make(288., 148.);
    const auto p4 = PointLonLat::make(108., -328.);

    // Check each of these is correctly shifted back to original point:
    const PointLonLat q2 = PointLonLat::make(p2.lon, p2.lat);
    const PointLonLat q3 = PointLonLat::make(p3.lon, p3.lat);
    const PointLonLat q4 = PointLonLat::make(p4.lon, p4.lat);

    EXPECT(p1.lon == q2.lon);
    EXPECT(p1.lat == q2.lat);
    EXPECT(p1.lon == q3.lon);
    EXPECT(p1.lat == q3.lat);
    EXPECT(p1.lon == q4.lon);
    EXPECT(p1.lat == q4.lat);

    // Check with longitude offset
    EXPECT(points_equal(PointLonLat::make(1., -90.), PointLonLat(0., -90.)));
    EXPECT(points_equal(PointLonLat::make(2., 90.), PointLonLat(0., 90.)));
    EXPECT(points_equal(PointLonLat::make(3., 180.), PointLonLat(183., 0.)));

    // Check with latitude offset
    constexpr double eps = 1.e-6;

    EXPECT(points_equal(PointLonLat::make(-1., 89.99999914622634, 0., eps), {0., 90.}));
    EXPECT(points_equal(PointLonLat::make(1., -89.99999914622634, 0., eps), {0., -90.}));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
