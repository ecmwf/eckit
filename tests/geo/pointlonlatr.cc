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
                 {1. + 42. * PointLonLatR::FULL_ANGLE, 0., 1.},
                 {1. - 42. * PointLonLatR::FULL_ANGLE, 0., 1.},
                 {1., 3. * PointLonLatR::FULL_ANGLE, 3. * PointLonLatR::FULL_ANGLE + 1.},
                 {-1., 3. * PointLonLatR::FULL_ANGLE, 4. * PointLonLatR::FULL_ANGLE - 1.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLatR::normalise_angle_to_minimum(test.angle, test.lim), PointLonLatR::EPS));
        }
    }


    SECTION("normalise_angle_to_maximum") {
        for (const auto& test : {
                 test_t{1., 0., 1. - PointLonLatR::FULL_ANGLE},
                 {1., 3. * PointLonLatR::FULL_ANGLE, 2. * PointLonLatR::FULL_ANGLE + 1.},
                 {-1., 3. * PointLonLatR::FULL_ANGLE, 3. * PointLonLatR::FULL_ANGLE - 1.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLatR::normalise_angle_to_maximum(test.angle, test.lim), PointLonLatR::EPS));
        }
    }
}


CASE("PointLonLatR normalisation") {
    PointLonLatR p(1, PointLonLatR::RIGHT_ANGLE);
    auto p2 = PointLonLatR::make(p.lonr, p.latr);
    auto p3 = PointLonLatR(1. + 42. * PointLonLatR::FULL_ANGLE, PointLonLatR::RIGHT_ANGLE);

    EXPECT_EQUAL(p2.lonr, 0.);
    EXPECT(points_equal(p, p2));
    EXPECT(points_equal(p, p3));

    PointLonLatR q(1., SOUTH_POLE_R.latr);
    auto q2 = q.antipode();
    auto q3 = q2.antipode();

    EXPECT_EQUAL(q2.lonr, 0.);
    EXPECT(points_equal(q2, p));
    EXPECT(points_equal(q3, q));
}


CASE("PointLonLatR comparison") {
    auto r(PointLonLatR::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    PointLonLatR a1{PointLonLatR::FLAT_ANGLE, 0.};
    PointLonLatR a2{-PointLonLatR::FLAT_ANGLE, 0.};
    EXPECT(points_equal(a1, a2));

    EXPECT(points_equal(SOUTH_POLE_R, {1., SOUTH_POLE_R.latr + PointLonLatR::FULL_ANGLE}));

    PointLonLatR c1{300., -30.};
    PointLonLatR c2{c1.lonr - PointLonLatR::FULL_ANGLE - PointLonLatR::EPS / 10.,
                    c1.latr + PointLonLatR::FULL_ANGLE + PointLonLatR::EPS / 10.};
    EXPECT(points_equal(c1, c2));

    EXPECT(points_equal(NORTH_POLE_R, {-42, PointLonLatR::RIGHT_ANGLE}));
    EXPECT(points_equal(SOUTH_POLE_R, {42., -PointLonLatR::RIGHT_ANGLE}));
}


CASE("PointLonLatR normalise angles") {
    EXPECT(types::is_approximately_equal(
        0., PointLonLatR::normalise_angle_to_minimum(0. + PointLonLatR::FULL_ANGLE, 0.), PointLonLatR::EPS));

    EXPECT(types::is_approximately_equal(
        1., PointLonLatR::normalise_angle_to_minimum(1. + PointLonLatR::FULL_ANGLE * 11, 0.), PointLonLatR::EPS));

    EXPECT(types::is_approximately_equal(
        2. + PointLonLatR::FULL_ANGLE * 11,
        PointLonLatR::normalise_angle_to_minimum(2. + PointLonLatR::FULL_ANGLE * 11, PointLonLatR::FULL_ANGLE * 11),
        PointLonLatR::EPS));
}


CASE("PointLonLatR conversion to/from PointLonLat") {
    PointLonLatR p{0., 0.};
    EXPECT(points_equal(p, PointLonLatR::make_from_lonlat(0., 0.)));

    PointLonLatR q{0., PointLonLatR::RIGHT_ANGLE};
    EXPECT(points_equal(q, PointLonLatR::make_from_lonlat(1., PointLonLat::RIGHT_ANGLE)));

    PointLonLatR r{42. * PointLonLatR::FULL_ANGLE, SOUTH_POLE_R.latr};
    EXPECT(points_equal(r, PointLonLatR::make_from_lonlat(0., SOUTH_POLE.lat - 42. * PointLonLat::FULL_ANGLE)));

    PointLonLatR s{10. * util::DEGREE_TO_RADIAN, 42. * PointLonLatR::FULL_ANGLE};
    EXPECT(points_equal(s, PointLonLatR::make_from_lonlat(10. - 42. * PointLonLat::FULL_ANGLE, 0.)));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
