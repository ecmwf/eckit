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


#include "eckit/geo/PointLonLatR.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("PointLonLatR normalise_angle_to_*") {
    struct test_t {
        double angle;
        double lim;
        double ref;
    };


    SECTION("normalise_angle_to_minimum") {
        for (const auto& test : {
                 test_t{1., 0., 1.},
                 {1. + 42. * PointLonLatR::GLOBE, 0., 1.},
                 {1. - 42. * PointLonLatR::GLOBE, 0., 1.},
                 {1., 3. * PointLonLatR::GLOBE, 3. * PointLonLatR::GLOBE + 1.},
                 {-1., 3. * PointLonLatR::GLOBE, 4. * PointLonLatR::GLOBE - 1.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLatR::normalise_angle_to_minimum(test.angle, test.lim), PointLonLatR::EPS));
        }
    }


    SECTION("normalise_angle_to_maximum") {
        for (const auto& test : {
                 test_t{1., 0., 1. - PointLonLatR::GLOBE},
                 {1., 3. * PointLonLatR::GLOBE, 2. * PointLonLatR::GLOBE + 1.},
                 {-1., 3. * PointLonLatR::GLOBE, 3. * PointLonLatR::GLOBE - 1.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLatR::normalise_angle_to_maximum(test.angle, test.lim), PointLonLatR::EPS));
        }
    }
}


CASE("PointLonLatR normalisation") {
    PointLonLatR p(1, PointLonLatR::NORTH_POLE);
    EXPECT_EQUAL(p.lonr, 1.);
    EXPECT_EQUAL(p.latr, PointLonLatR::NORTH_POLE);

    auto p2 = PointLonLatR::make(p.lonr, p.latr);
    EXPECT_EQUAL(p2.lonr, 0.);
    EXPECT(points_equal(p, p2));

    auto p3 = PointLonLatR(1. + 42. * PointLonLatR::GLOBE, PointLonLatR::NORTH_POLE);
    EXPECT(points_equal(p, p3));

    PointLonLatR q(1., PointLonLatR::SOUTH_POLE);
    EXPECT_EQUAL(q.lonr, 1.);
    EXPECT_EQUAL(q.latr, PointLonLatR::SOUTH_POLE);

    auto q2 = q.antipode();
    EXPECT_EQUAL(q2.lonr, 0.);
    EXPECT(points_equal(q2, p));

    auto q3 = q2.antipode();
    EXPECT(points_equal(q3, q));
}


CASE("PointLonLatR comparison") {
    auto r(PointLonLatR::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    PointLonLatR a1{PointLonLatR::ANTIMERIDIAN, 0.};
    PointLonLatR a2{-PointLonLatR::ANTIMERIDIAN, 0.};
    EXPECT(points_equal(a1, a2));

    PointLonLatR b1{0., PointLonLatR::SOUTH_POLE};
    auto b2 = PointLonLatR::make(1., PointLonLatR::SOUTH_POLE + PointLonLatR::GLOBE);
    EXPECT(points_equal(b1, b2));

    PointLonLatR c1{300., -30.};
    PointLonLatR c2{c1.lonr - PointLonLatR::GLOBE - PointLonLatR::EPS / 10.,
                    c1.latr + PointLonLatR::GLOBE + PointLonLatR::EPS / 10.};
    EXPECT(points_equal(c1, c2));

    PointLonLatR e1{PointLonLatR::GREENWICH, PointLonLatR::NORTH_POLE};
    PointLonLatR e2{PointLonLatR::ANTIMERIDIAN, PointLonLatR::NORTH_POLE};
    EXPECT(points_equal(e1, e2));

    PointLonLatR f1{PointLonLatR::GREENWICH, PointLonLatR::SOUTH_POLE};
    PointLonLatR f2{-PointLonLatR::ANTIMERIDIAN, PointLonLatR::SOUTH_POLE};
    EXPECT(points_equal(f1, f2));
}


CASE("PointLonLatR normalise angles") {
    EXPECT(types::is_approximately_equal(0., PointLonLatR::normalise_angle_to_minimum(0. + PointLonLatR::GLOBE, 0.),
                                         PointLonLatR::EPS));

    EXPECT(types::is_approximately_equal(
        1., PointLonLatR::normalise_angle_to_minimum(1. + PointLonLatR::GLOBE * 11, 0.), PointLonLatR::EPS));

    EXPECT(types::is_approximately_equal(
        2. + PointLonLatR::GLOBE * 11,
        PointLonLatR::normalise_angle_to_minimum(2. + PointLonLatR::GLOBE * 11, PointLonLatR::GLOBE * 11),
        PointLonLatR::EPS));
}


CASE("PointLonLatR conversion to/from PointLonLat") {
    PointLonLatR p{0., 0.};
    EXPECT(points_equal(p, PointLonLatR::make_from_lonlat(0., 0.)));

    PointLonLatR q{0., PointLonLatR::NORTH_POLE};
    EXPECT(points_equal(q, PointLonLatR::make_from_lonlat(1., PointLonLat::NORTH_POLE)));

    PointLonLatR r{42. * PointLonLatR::GLOBE, PointLonLatR::SOUTH_POLE};
    EXPECT(points_equal(r, PointLonLatR::make_from_lonlat(0., PointLonLat::SOUTH_POLE - 42. * PointLonLat::GLOBE)));

    PointLonLatR s{10. * util::DEGREE_TO_RADIAN, 42. * PointLonLatR::GLOBE};
    EXPECT(points_equal(s, PointLonLatR::make_from_lonlat(10. - 42. * PointLonLat::GLOBE, 0.)));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
