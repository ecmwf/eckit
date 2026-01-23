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

#include "eckit/geo/PointXYZ.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("PointXYZ initialisation") {
    PointXYZ z;

    EXPECT(z.X() == 0.);
    EXPECT(z.Y() == 0.);
    EXPECT(z.Z() == 0.);

    PointXYZ p = {1., 2., 3.};
    PointXYZ s(p);

    EXPECT(s.X() == 1.);
    EXPECT(s.Y() == 2.);
    EXPECT(s.Z() == 3.);
}


CASE("PointXYZ addition") {
    PointXYZ p1 = {1., 2., 3.};
    PointXYZ p2{1., 2., 3.};

    PointXYZ r = p1 + p2;

    EXPECT(r.X() == 2.);
    EXPECT(r.Y() == 4.);
    EXPECT(r.Z() == 6.);
}


CASE("PointXYZ subtraction") {
    PointXYZ p1{2., 5., 7.};
    PointXYZ p2{1., 2., 3.};

    PointXYZ r = p1 - p2;

    EXPECT(r.X() == 1.);
    EXPECT(r.Y() == 3.);
    EXPECT(r.Z() == 4.);
}


CASE("PointXYZ scaling") {
    PointXYZ p1{1., 2., 3.};

    PointXYZ r = p1 * 42.;

    EXPECT(r.X() == 42.);
    EXPECT(r.Y() == 84.);
    EXPECT(r.Z() == 126.);
}


CASE("PointXYZ equality") {
    PointXYZ p1{1., 2., 3.};
    PointXYZ p2{1., 2., 3.};

    EXPECT(p1 == p2);
}


CASE("PointXYZ inequality") {
    PointXYZ p1{1., 2., 3.};
    PointXYZ p2{1., 2., 4.};

    EXPECT(p1 != p2);
}


CASE("PointXYZ distance comparison") {
    PointXYZ p1{2., 1., 0.};
    PointXYZ p2{1., 2., 4.};
    PointXYZ p3{5., 5., 5.};

    EXPECT(types::is_approximately_equal(std::sqrt(18.), p1.distance(p2)));
    EXPECT(types::is_approximately_equal(std::sqrt(50.), p1.distance(p3)));
}


CASE("PointXYZ distance2 comparison") {
    PointXYZ p1{2., 1., 0.};
    PointXYZ p2{1., 2., 4.};
    PointXYZ p3;

    EXPECT(types::is_approximately_equal(p1.distance2(p2), 18.));
    EXPECT(types::is_approximately_equal(p1.distance2(p3), 5.));

    EXPECT(p2.distance2(p1) > p3.distance2(p1));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
