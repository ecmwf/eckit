/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::geometry;

namespace eckit {
namespace test {

CASE("Inits to Zero") {
    Point2 q;

    EXPECT(q[XX] == 0.);
    EXPECT(q[YY] == 0.);
}

CASE("Inits to Array/Point") {
    Point2 q = {4.0, 5.0};

    EXPECT(q[XX] == 4.0);
    EXPECT(q[YY] == 5.0);

    EXPECT(q[LON] == 4.0);
    EXPECT(q[LAT] == 5.0);

    Point2 r(q);
    EXPECT(r[XX] == 4.0);
    EXPECT(r[YY] == 5.0);

    Point3 p = {1.0, 2.0, 3.0};
    Point3 s(p);
    EXPECT(s[XX] == 1.0);
    EXPECT(s[YY] == 2.0);
    EXPECT(s[ZZ] == 3.0);
}

CASE("Point2 addition") {
    Point2 p1 = {1.0, 2.0};
    Point2 p2 = {2.0, 4.0};

    Point2 r = p1 + p2;

    EXPECT(r[XX] == 3.0);
    EXPECT(r[YY] == 6.0);
}

CASE("Point2 subtraction") {
    Point2 p1 = {2.0, 5.0};
    Point2 p2 = {1.0, 2.0};

    Point2 r = p1 - p2;

    EXPECT(r[XX] == 1.0);
    EXPECT(r[YY] == 3.0);
}

CASE("Point2 scaling") {
    Point2 p1 = {1.0, 2.0};
    Point2 p2(p1);

    Point2 r = p1 * 42.0;

    EXPECT(r[XX] == 42.0);
    EXPECT(r[YY] == 84.0);

    Point2 oo;

    Point2 p3 = p2 * 2.0;
    Point2 p4 = p3 + p2;
    Point2 p5 = p4-p2*3;
    EXPECT(p5 == oo);
}

CASE("Point2 equality") {
    Point2 p1 = {1.0, 2.0};
    Point2 p2 = {1.0, 2.0};

    EXPECT(p1 == p2);
}

CASE("Point2 inequality") {
    Point2 p1 = {1.0, 3.0};
    Point2 p2 = {1.0, 4.0};

    EXPECT(p1 != p2);
}

CASE("Point2 comparison") {
    Point2 p1 = {2.0, 1.0};
    Point2 p2 = {1.0, 2.0};

    EXPECT(p2 < p1);
}

CASE("Point distance comparison") {
    Point2 p1 = {2.0, 1.0};
    Point2 p2 = {1.0, 2.0};

    EXPECT(p1.distance(p2) < 1.5);
    EXPECT(1.4 < p1.distance(p2));

    Point2 p3 = {5.0, 5.0};
    EXPECT(p1.distance(p3) < 5.0 + std::numeric_limits<float>::epsilon());
    EXPECT(5.0 - std::numeric_limits<float>::epsilon() < p1.distance(p3));
}

CASE("Point distance2 comparison") {
    Point2 p1 = {2.0, 1.0};
    Point2 p2 = {1.0, 2.0};

    EXPECT(p1.distance2(p2) < 2.0 + std::numeric_limits<float>::epsilon());

    Point2 p3 = {5.0, 5.0};
    EXPECT(p1.distance2(p3) < 25.0 + std::numeric_limits<float>::epsilon());
    EXPECT(25.0 - std::numeric_limits<float>::epsilon() < p1.distance2(p3));
}

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
