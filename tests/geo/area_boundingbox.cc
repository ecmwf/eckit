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


#include "eckit/geo/area/BoundingBox.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("global") {
    area::BoundingBox a;
    area::BoundingBox b(90, 0, -90, 360);
    EXPECT_EQUAL(a, b);
}


CASE("latitude (checks)") {
    EXPECT_THROWS(area::BoundingBox(-90, 0, 90, 360));  // fails South<=North
    EXPECT_NO_THROW(area::BoundingBox(90, 0, 90, 360));
    EXPECT_NO_THROW(area::BoundingBox(-90, 0, -90, 360));
}


CASE("longitude (normalisation)") {
    for (auto west : {-900, -720, -540, -360, -180, 0, 180, 360, 540, 720, 900}) {
        area::BoundingBox a(90, west, 90, west);
        EXPECT_EQUAL(a.west, west);

        area::BoundingBox b(90, west, 90, west - 1);
        EXPECT_EQUAL(b.east, west + 360 - 1);
    }
}


CASE("assignment") {
    area::BoundingBox a(10, 1, -10, 100);

    area::BoundingBox b(20, 2, -20, 200);
    EXPECT_NOT_EQUAL(a.north, b.north);
    EXPECT_NOT_EQUAL(a, b);

    b = a;
    EXPECT_EQUAL(a.north, b.north);
    EXPECT_EQUAL(a, b);

    b.north = 30;
    EXPECT_EQUAL(b.north, 30);
    EXPECT_EQUAL(a.north, 10);

    area::BoundingBox c(a);
    EXPECT_EQUAL(a.north, c.north);
    EXPECT_EQUAL(a, c);

    c.north = 40;
    EXPECT_EQUAL(c.north, 40);
    EXPECT_EQUAL(a.north, 10);

    auto d(std::move(a));
    EXPECT_EQUAL(d.north, 10);

    d.north = 50;
    EXPECT_EQUAL(d.north, 50);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}