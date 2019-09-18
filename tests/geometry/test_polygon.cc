/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/geometry/Polygon.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::geometry;

namespace eckit {
namespace test {

CASE("empty polygon") {
    Polygon poly1;
    Polygon poly2;

    EXPECT(poly1.sameAs(poly2));
}

CASE("equality") {
    Polygon poly1;
    Polygon poly2;

    EXPECT(poly1.num_vertices() == 0);

    Point2 p1 = {1.0, 2.0};
    poly1.push_front(p1);
    EXPECT(poly1.num_vertices() == 1);
    EXPECT(poly1.vertex(0) == p1);

    EXPECT(!poly1.sameAs(poly2));

    poly2.push_back(p1);
    EXPECT(poly1.sameAs(poly2));

    Point2 p2 = {2.0, 1.0};
    poly1.push_front(p2);
    EXPECT(!poly1.sameAs(poly2));

    poly2.push_back(p2);
    EXPECT(!poly1.sameAs(poly2));
}

CASE("congruence") {
    Polygon poly1;
    Polygon poly2;
    EXPECT(poly1.congruent(poly2));

    Point2 p1 = {1.0, 2.0};
    poly1.push_front(p1);
    EXPECT(!poly1.congruent(poly2));

    poly2.push_back(p1);
    EXPECT(poly1.congruent(poly2));

    Point2 p2 = {2.0, 1.0};
    poly1.push_front(p2);
    EXPECT(!poly1.congruent(poly2));

    poly2.push_back(p2);
    EXPECT(poly1.congruent(poly2));

    Point2 p3 = {3.0, 4.0};
    poly2.push_back(p3);
    Polygon poly3 = {p2, p3, p1};
    EXPECT(!poly2.sameAs(poly3));
    EXPECT(poly2.congruent(poly3));

    EXPECT(poly2.num_vertices() == 3);
    EXPECT(poly2.vertex(2) == poly3.vertex(1));

    Polygon poly4 = {p3, p1, p2};
    EXPECT(!poly2.sameAs(poly4));
    EXPECT(poly2.congruent(poly4));
}

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
