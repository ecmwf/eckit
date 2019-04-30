/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/Point3.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::geometry;

namespace eckit {
namespace test {

CASE("KPoint Inits to Zero") {
    Point3 p;

    EXPECT(p[XX] == 0.);
    EXPECT(p[YY] == 0.);
    EXPECT(p[ZZ] == 0.);
}

CASE("KPoint Inits to Array") {
    Point3 p = {1.0, 2.0, 3.0};

    EXPECT(p[XX] == 1.0);
    EXPECT(p[YY] == 2.0);
    EXPECT(p[ZZ] == 3.0);
}

CASE("KPoint addition") {
    Point3 p1 = {1.0, 2.0, 3.0};
    Point3 p2 = {1.0, 2.0, 3.0};

    Point3 r = p1 + p2;

    EXPECT(r[XX] == 2.0);
    EXPECT(r[YY] == 4.0);
    EXPECT(r[ZZ] == 6.0);
}

CASE("KPoint subtraction") {
    Point3 p1 = {2.0, 5.0, 7.0};
    Point3 p2 = {1.0, 2.0, 3.0};

    Point3 r = p1 - p2;

    EXPECT(r[XX] == 1.0);
    EXPECT(r[YY] == 3.0);
    EXPECT(r[ZZ] == 4.0);
}

CASE("KPoint subtraction") {
    Point3 p1 = {2.0, 5.0, 7.0};
    Point3 p2 = {1.0, 2.0, 3.0};

    Point3 r = p1 - p2;

    EXPECT(r[XX] == 1.0);
    EXPECT(r[YY] == 3.0);
    EXPECT(r[ZZ] == 4.0);
}

CASE("KPoint scaling") {
    Point3 p1 = {1.0, 2.0, 3.0};

    Point3 r = p1 * 42.0;

    EXPECT(r[XX] == 42.0);
    EXPECT(r[YY] == 84.0);
    EXPECT(r[ZZ] == 126.0);
}

CASE("KPoint equality") {
    Point3 p1 = {1.0, 2.0, 3.0};
    Point3 p2 = {1.0, 2.0, 3.0};

    EXPECT(p1 == p2);
}

CASE("KPoint inequality") {
    Point3 p1 = {1.0, 2.0, 3.0};
    Point3 p2 = {1.0, 2.0, 4.0};

    EXPECT(p1 != p2);
}

CASE("KPoint comparison") {
    Point3 p1 = {2.0, 1.0, 0.0};
    Point3 p2 = {1.0, 2.0, 4.0};

    EXPECT(p2 < p1);
}

CASE("KPoint distance2 comparison") {
    Point3 zz;
    Point3 p1 = {2.0, 1.0, 0.0};
    Point3 p2 = {1.0, 2.0, 4.0};

    EXPECT(p1.distance2(zz) < p2.distance2(zz));
}

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
