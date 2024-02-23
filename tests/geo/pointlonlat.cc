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
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}