/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "eckit/geo/CoordinateHelpers.h"
#include "eckit/geo/Point2.h"
#include "eckit/testing/Test.h"


namespace eckit::test {

using namespace geo;

// -----------------------------------------------------------------------------

CASE("normalise angles") {
    EXPECT(0. == normalise_angle(360., 0.));
    EXPECT(14. == normalise_angle(374., 0.));
    EXPECT(14. == normalise_angle(374., -90.));
    EXPECT(374. == normalise_angle(374., 90.));
}

CASE("canonicalise on sphere") {
    using types::is_approximately_equal;

    const Point2 p1(108., 32.);

    // Worse coordinates for the same point:
    const Point2 p2(-252., 32.);
    const Point2 p3(288., 148.);
    const Point2 p4(108., -328.);

    // Check each of these is correctly shifted back to original point:
    const Point2 q2 = canonicaliseOnSphere(p2);
    const Point2 q3 = canonicaliseOnSphere(p3);
    const Point2 q4 = canonicaliseOnSphere(p4);

    EXPECT(p1.x() == q2.x());
    EXPECT(p1.y() == q2.y());
    EXPECT(p1.x() == q3.x());
    EXPECT(p1.y() == q3.y());
    EXPECT(p1.x() == q4.x());
    EXPECT(p1.y() == q4.y());

    // Check with longitude offset
    const Point2 q2b = canonicaliseOnSphere(p2, -90.);
    EXPECT(q2b.x() == 108.);
    EXPECT(q2b.y() == 32.);

    const Point2 q2c = canonicaliseOnSphere(p2, 90.);
    EXPECT(q2c.x() == 108.);
    EXPECT(q2c.y() == 32.);

    const Point2 q2d = canonicaliseOnSphere(p2, 180.);
    EXPECT(q2d.x() == 468.);
    EXPECT(q2d.y() == 32.);
}

// -----------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
