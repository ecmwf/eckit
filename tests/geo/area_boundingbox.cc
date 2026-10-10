// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("AreaFactory::make_from_string") {
    const area::BoundingBox expected_area;
    const std::string expected_spec = expected_area.spec_str();

    for (const auto& spec : std::vector<std::string>{
             "{}",
             "{north: 90, south: -90, west: 0, east: 360}",
             "{type: bounding_box}",
             "{north: 90}",
         }) {
        std::unique_ptr<const Area> area(geo::AreaFactory::make_from_string(spec));

        EXPECT_EQUAL(area->spec_str(), expected_spec);
        EXPECT(expected_area == *area);
    }

    std::unique_ptr<const Area> area1(
        geo::AreaFactory::make_from_string(R"({north: 90, west: 0, south: -90, east: 360})"));

    EXPECT_EQUAL(area1->spec_str(), expected_spec);

    std::unique_ptr<const Area> area2(
        geo::AreaFactory::make_from_string(R"({north: 10, west: 35641, south: 0, east: 15130})"));

    EXPECT_EQUAL(area2->spec_str(), R"({"area":[10,1,0,10]})");
}


CASE("Area: spec, and as part of a grid's") {
    struct test_t {
        std::string area;
        std::string spec;
        std::string grid_spec;
    };

    for (const auto& test : std::vector<test_t>{
             {"{}", R"({"area":[90,0,-90,360]})", "{}"},
             {"{area: [10, 1, 0, 10]}", R"({"area":[10,1,0,10]})", R"({"area":[10,1,0,10]})"},
             {"{type: bounding_box_xy, bounding_box_xy: [0, 0, 1, 1]}",
              R"({"bounding_box_xy":[0,0,1,1],"type":"bounding_box_xy"})",
              R"({"area":{"bounding_box_xy":[0,0,1,1],"type":"bounding_box_xy"}})"},
         }) {
        std::unique_ptr<const Area> area(AreaFactory::make_from_string(test.area));
        EXPECT_EQUAL(area->spec_str(), test.spec);

        spec::Custom grid_spec;
        area->fill_grid_spec(grid_spec);
        EXPECT_EQUAL(grid_spec.str(), test.grid_spec);
    }
}


CASE("global") {
    area::BoundingBox a;
    area::BoundingBox b(90, 0, -90, 360);
    EXPECT(a == b);
}


CASE("latitude (checks)") {
    EXPECT_THROWS(area::BoundingBox(-90, 0, 90, 360));  // fails South<=North
    EXPECT_NO_THROW(area::BoundingBox(90, 0, 90, 360));
    EXPECT_NO_THROW(area::BoundingBox(-90, 0, -90, 360));
}


CASE("longitude (normalisation)") {
    for (double west : {-900, -720, -540, -360, -180, 0, 180, 360, 540, 720, 900}) {
        area::BoundingBox a(90, west, 90, west);

        EXPECT_EQUAL(a.west(), west);
        EXPECT(a.empty());

        area::BoundingBox b{90, west, -90, west - 1};
        auto c = area::BoundingBox::make_from_area(90, west + 42 * 360., -90, west - 42 * 360. - 1);
        ASSERT(c);

        EXPECT(c->east() == c->west() + 360 - 1);
        EXPECT(b == *c);
    }
}


CASE("assignment") {
    area::BoundingBox a(10, 1, -10, 100);
    area::BoundingBox b(20, 2, -20, 200);

    EXPECT_NOT_EQUAL(a.north(), b.north());
    EXPECT(a != b);

    b = a;

    EXPECT_EQUAL(a.north(), b.north());
    EXPECT(a == b);

    b = {30., b.west(), b.south(), b.east()};

    EXPECT_EQUAL(b.north(), 30);
    EXPECT_EQUAL(a.north(), 10);

    area::BoundingBox c(a);

    EXPECT_EQUAL(a.north(), c.north());
    EXPECT(a == c);

    c = {40., c.west(), c.south(), c.east()};

    EXPECT_EQUAL(c.north(), 40);
    EXPECT_EQUAL(a.north(), 10);

    auto d(std::move(a));

    EXPECT_EQUAL(d.north(), 10);

    d = {50., d.west(), d.south(), d.east()};

    EXPECT_EQUAL(d.north(), 50);
}


CASE("comparison") {
    area::BoundingBox a(10, 1, -10, 100);
    area::BoundingBox b(20, 2, -20, 200);

    EXPECT(!area::bounding_box_equal(a, b));

    for (const auto& c : {a, b}) {
        const area::BoundingBox d{c.north(), c.west() + 42 * PointLonLat::FULL_ANGLE, c.south(),
                                  c.east() + 41 * PointLonLat::FULL_ANGLE};
        EXPECT(area::bounding_box_equal(c, d));
    }
}


CASE("properties") {
    area::BoundingBox a{10, 1, -10, 100};
    area::BoundingBox b{20, 2, -20, 200};

    auto c = area::BoundingBox::make_global_prime();
    ASSERT(c);

    auto d = area::BoundingBox::make_global_antiprime();
    ASSERT(d);

    area::BoundingBox e;

    for (const auto& bb : {a, b, *c, *d, e}) {
        EXPECT(!bb.empty());
        EXPECT(bb.contains(PointLonLat{10, 0}));
        EXPECT(bb.global() == bb.contains(PointLonLat{0, 0}));
        EXPECT(bb.global() == (bb.periodic() && bb.contains(NORTH_POLE) && bb.contains(SOUTH_POLE)));
    }
}


CASE("intersects") {
    area::BoundingBox a(10, 1, -10, 100);
    area::BoundingBox b(20, 2, -20, 200);

    EXPECT(!area::bounding_box_equal(a, b));

    for (const auto& c : {a, b}) {
        const area::BoundingBox d{c.north(), c.west() + 42 * PointLonLat::FULL_ANGLE, c.south(),
                                  c.east() + 41 * PointLonLat::FULL_ANGLE};
        EXPECT(area::bounding_box_equal(c, d));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
