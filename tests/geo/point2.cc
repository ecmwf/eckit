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

#include "eckit/geo/Point2.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("Point2 initialisation") {
    Point2 z;

    EXPECT(z.X == 0.);
    EXPECT(z.Y == 0.);

    Point2 q{4., 5.};

    EXPECT(q.X == 4.);
    EXPECT(q.Y == 5.);

    Point2 r(q);

    EXPECT(r.X == 4.);
    EXPECT(r.Y == 5.);
}


CASE("Point2 addition") {
    Point2 p1{1., 2.};
    Point2 p2{2., 4.};

    Point2 r = p1 + p2;

    EXPECT(r.X == 3.);
    EXPECT(r.Y == 6.);
}


CASE("Point2 subtraction") {
    Point2 p1{2., 5.};
    Point2 p2{1., 2.};

    Point2 r = p1 - p2;

    EXPECT(r.X == 1.);
    EXPECT(r.Y == 3.);
}


CASE("Point2 scaling") {
    Point2 p1{1., 2.};
    Point2 p2(p1);

    Point2 r = p1 * 42.;

    EXPECT(r.X == 42.);
    EXPECT(r.Y == 84.);

    Point2 oo;

    Point2 p3 = p2 * 2.;
    Point2 p4 = p3 + p2;
    Point2 p5 = p4 - p2 * 3;
    EXPECT(p5 == oo);
}


CASE("Point2 equality") {
    Point2 p1{1., 2.};
    Point2 p2{1., 2.};

    EXPECT(p1 == p2);
}


CASE("Point2 inequality") {
    Point2 p1{1., 3.};
    Point2 p2{1., 4.};

    EXPECT(p1 != p2);
}


CASE("Point2 distance comparison") {
    Point2 p1{2., 1.};
    Point2 p2{1., 2.};
    Point2 p3{5., 5.};

    EXPECT(types::is_approximately_equal(std::sqrt(2.), p1.distance(p2)));
    EXPECT(types::is_approximately_equal(5., p1.distance(p3)));
}


CASE("Point2 distance2 comparison") {
    Point2 p1{2., 1.};
    Point2 p2{1., 2.};
    Point2 p3{5., 5.};

    EXPECT(types::is_approximately_equal(p1.distance2(p2), 2.));
    EXPECT(types::is_approximately_equal(p1.distance2(p3), 25.));

    EXPECT(p2.distance2(p1) < p3.distance2(p1));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
