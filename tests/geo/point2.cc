/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2..
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <cmath>

#include "eckit/geo/PointXY.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("PointXY initialisation") {
    PointXY z;

    EXPECT(z.X == 0.);
    EXPECT(z.Y == 0.);

    PointXY q{4., 5.};

    EXPECT(q.X == 4.);
    EXPECT(q.Y == 5.);

    PointXY r(q);

    EXPECT(r.X == 4.);
    EXPECT(r.Y == 5.);
}


CASE("PointXY addition") {
    PointXY p1{1., 2.};
    PointXY p2{2., 4.};

    PointXY r = p1 + p2;

    EXPECT(r.X == 3.);
    EXPECT(r.Y == 6.);
}


CASE("PointXY subtraction") {
    PointXY p1{2., 5.};
    PointXY p2{1., 2.};

    PointXY r = p1 - p2;

    EXPECT(r.X == 1.);
    EXPECT(r.Y == 3.);
}


CASE("PointXY scaling") {
    PointXY p1{1., 2.};
    PointXY p2(p1);

    PointXY r = p1 * 42.;

    EXPECT(r.X == 42.);
    EXPECT(r.Y == 84.);

    PointXY oo;

    PointXY p3 = p2 * 2.;
    PointXY p4 = p3 + p2;
    PointXY p5 = p4 - p2 * 3;
    EXPECT(p5 == oo);
}


CASE("PointXY equality") {
    PointXY p1{1., 2.};
    PointXY p2{1., 2.};

    EXPECT(p1 == p2);
}


CASE("PointXY inequality") {
    PointXY p1{1., 3.};
    PointXY p2{1., 4.};

    EXPECT(p1 != p2);
}


CASE("PointXY distance comparison") {
    PointXY p1{2., 1.};
    PointXY p2{1., 2.};
    PointXY p3{5., 5.};

    EXPECT(types::is_approximately_equal(std::sqrt(2.), p1.distance(p2)));
    EXPECT(types::is_approximately_equal(5., p1.distance(p3)));
}


CASE("PointXY distance2 comparison") {
    PointXY p1{2., 1.};
    PointXY p2{1., 2.};
    PointXY p3{5., 5.};

    EXPECT(types::is_approximately_equal(p1.distance2(p2), 2.));
    EXPECT(types::is_approximately_equal(p1.distance2(p3), 25.));

    EXPECT(p2.distance2(p1) < p3.distance2(p1));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
