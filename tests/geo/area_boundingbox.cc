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


#include <memory>

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
    for (double west : {-900, -720, -540, -360, -180, 0, 180, 360, 540, 720, 900}) {
        area::BoundingBox a(90, west, 90, west);

        EXPECT_EQUAL(a.west, west);
        EXPECT(a.empty());

        area::BoundingBox b{90, west, -90, west - 1};
        std::unique_ptr<area::BoundingBox> c(
            area::BoundingBox::make_from_area(90, west + 42 * 360., -90, west - 42 * 360. - 1));

        EXPECT(c->east == c->west + 360 - 1);
        EXPECT(b == *c);
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

    b = {30., b.west, b.south, b.east};

    EXPECT_EQUAL(b.north, 30);
    EXPECT_EQUAL(a.north, 10);

    area::BoundingBox c(a);

    EXPECT_EQUAL(a.north, c.north);
    EXPECT_EQUAL(a, c);

    c = {40., c.west, c.south, c.east};

    EXPECT_EQUAL(c.north, 40);
    EXPECT_EQUAL(a.north, 10);

    auto d(std::move(a));

    EXPECT_EQUAL(d.north, 10);

    d = {50., d.west, d.south, d.east};

    EXPECT_EQUAL(d.north, 50);
}


CASE("comparison") {
    area::BoundingBox a(10, 1, -10, 100);
    area::BoundingBox b(20, 2, -20, 200);

    EXPECT(!area::bounding_box_equal(a, b));

    for (const auto& c : {a, b}) {
        const area::BoundingBox d{c.north, c.west + 42 * PointLonLat::FULL_ANGLE, c.south,
                                  c.east + 41 * PointLonLat::FULL_ANGLE};
        EXPECT(area::bounding_box_equal(c, d));
    }
}


CASE("properties") {
    area::BoundingBox a{10, 1, -10, 100};
    area::BoundingBox b{20, 2, -20, 200};
    std::unique_ptr<area::BoundingBox> c(area::BoundingBox::make_global_prime());
    std::unique_ptr<area::BoundingBox> d(area::BoundingBox::make_global_antiprime());
    area::BoundingBox e;

    for (const auto& bb : {a, b, *c, *d, e}) {
        EXPECT(!bb.empty());
        EXPECT(bb.contains({10, 0}));
        EXPECT(bb.global() == bb.contains({0, 0}));
        EXPECT(bb.global() == (bb.periodic() && bb.contains(NORTH_POLE) && bb.contains(SOUTH_POLE)));
    }
}


CASE("intersects") {
    area::BoundingBox a(10, 1, -10, 100);
    area::BoundingBox b(20, 2, -20, 200);

    EXPECT(!area::bounding_box_equal(a, b));

    for (const auto& c : {a, b}) {
        const area::BoundingBox d{c.north, c.west + 42 * PointLonLat::FULL_ANGLE, c.south,
                                  c.east + 41 * PointLonLat::FULL_ANGLE};
        EXPECT(area::bounding_box_equal(c, d));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
