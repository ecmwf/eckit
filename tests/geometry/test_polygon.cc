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
#include "eckit/geometry/polygon/LonLatPolygon.h"
#include "eckit/geometry/polygon/Polygon.h"
#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

CASE("Polygon") {
    using geometry::Point2;
    using geometry::polygon::Polygon;

    SECTION("empty polygon") {
        Polygon poly1;
        Polygon poly2;

        EXPECT(poly1.sameAs(poly2));
    }

    SECTION("equality") {
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

    SECTION("congruence") {
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
}

CASE("LonLatPolygon") {
    using PointLonLat = geometry::Point2;
    using Polygon     = geometry::polygon::LonLatPolygon;

    SECTION("removeAlignedPoints") {
        const std::vector<PointLonLat> points1{{0, 0}, {1, 1}, {2, 2}, {0, 0}};
        EXPECT_EQUAL(Polygon(points1, false).size(), 4);
        EXPECT_EQUAL(Polygon(points1, true).size(), 2);

        const std::vector<PointLonLat> points2{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}, {0, 0}};
        EXPECT_EQUAL(Polygon(points2, false).size(), 9);
        EXPECT_EQUAL(Polygon(points2, true).size(), 5);
    }

    SECTION("contains North pole") {
        Polygon poly({{0, 90}, {0, 0}, {1, 0}, {1, 90}, {0, 90}});
        EXPECT(poly.contains({0, 90}));
        EXPECT(poly.contains({10, 90}));
        EXPECT_NOT(poly.contains({0, -90}));
        EXPECT_NOT(poly.contains({10, -90}));
    }

    SECTION("contains South pole") {
        Polygon poly({{0, -90}, {0, 0}, {1, 0}, {1, -90}, {0, -90}});
        EXPECT_NOT(poly.contains({0, 90}));
        EXPECT_NOT(poly.contains({10, 90}));
        EXPECT(poly.contains({0, -90}));
        EXPECT(poly.contains({10, -90}));
    }

    SECTION("contains South and North poles") {
        Polygon poly({{0, -90}, {0, 90}, {1, 90}, {1, -90}, {0, -90}});
        EXPECT(poly.contains({0, 90}));
        EXPECT(poly.contains({10, 90}));
        EXPECT(poly.contains({0, 0}));
        EXPECT_NOT(poly.contains({10, 0}));
        EXPECT(poly.contains({0, -90}));
        EXPECT(poly.contains({10, -90}));
    }

    SECTION("MIR-566: wide polygon") {
        Polygon poly1({{0, 0}, {361, 0}, {361, 2}, {0, 2}, {0, 0}});
        EXPECT(poly1.contains({0, 1}));
        EXPECT(poly1.contains({2, 1}));
        EXPECT_NOT(poly1.contains({362, 1}));
        EXPECT_NOT(poly1.contains({722, 1}));

        Polygon poly2({{0, 0}, {11, 0}, {11, 2}, {0, 2}, {0, 0}});
        EXPECT(poly2.contains({0, 1}));
        EXPECT(poly2.contains({2, 1}));
        EXPECT(poly2.contains({362, 1}));
        EXPECT(poly2.contains({722, 1}));

        Polygon poly3({{0, 0}, {360, 0}, {360, 2}, {0, 2}, {0, 0}});
        EXPECT(poly3.contains({0, 1}));
        EXPECT(poly3.contains({2, 1}));
        EXPECT_NOT(poly3.contains({362, 1}));
        EXPECT_NOT(poly3.contains({722, 1}));
    }

    SECTION("MIR-566: winding number strict check of edges") {
        Polygon poly({{110, -34}, {90, -62}, {100, -59}, {110, -50}, {132, -40}, {110, -34}});
        EXPECT_NOT(poly.contains({90, -40}));
    }

    SECTION("Edges of simple rectangular polygon") {
        double lonmin = 0;
        double lonmax = 360;
        double latmax = 80;
        double latmin = 0;

        Polygon poly({{lonmin, latmax}, {lonmax, latmax}, {lonmax, latmin}, {lonmin, latmin}, {lonmin, latmax}});

        double lonmid = 0.5 * (lonmin + lonmax);
        double latmid = 0.5 * (latmin + latmax);

        EXPECT(poly.contains({lonmin, latmax}));  // FAILS!
        EXPECT(poly.contains({lonmid, latmax}));  // FAILS!
        EXPECT(poly.contains({lonmax, latmax}));  // FAILS!
        EXPECT(poly.contains({lonmax, latmid}));
        EXPECT(poly.contains({lonmax, latmin}));
        EXPECT(poly.contains({lonmid, latmin}));  // FAILS!
        EXPECT(poly.contains({lonmin, latmin}));
        EXPECT(poly.contains({lonmin, latmid}));
    }
}

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
