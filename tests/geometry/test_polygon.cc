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

        Polygon::value_type p1 = {1.0, 2.0};
        poly1.push_front(p1);
        EXPECT(poly1.num_vertices() == 1);
        EXPECT(poly1.vertex(0) == p1);

        EXPECT(!poly1.sameAs(poly2));

        poly2.push_back(p1);
        EXPECT(poly1.sameAs(poly2));

        Polygon::value_type p2 = {2.0, 1.0};
        poly1.push_front(p2);
        EXPECT(!poly1.sameAs(poly2));

        poly2.push_back(p2);
        EXPECT(!poly1.sameAs(poly2));
    }

    SECTION("congruence") {
        Polygon poly1;
        Polygon poly2;
        EXPECT(poly1.congruent(poly2));

        Polygon::value_type p1 = {1.0, 2.0};
        poly1.push_front(p1);
        EXPECT(!poly1.congruent(poly2));

        poly2.push_back(p1);
        EXPECT(poly1.congruent(poly2));

        Polygon::value_type p2 = {2.0, 1.0};
        poly1.push_front(p2);
        EXPECT(!poly1.congruent(poly2));

        poly2.push_back(p2);
        EXPECT(poly1.congruent(poly2));

        Polygon::value_type p3 = {3.0, 4.0};
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
    using Polygon = geometry::polygon::LonLatPolygon;

    SECTION("construction") {
        const std::vector<Polygon::value_type> points1{{0, 0}, {1, 1}, {2, 2}, {0, 0}};
        const std::vector<Polygon::value_type> points2{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {1, 2}, {0, 2}, {0, 1}, {0, 0}};

        EXPECT_EQUAL(Polygon(points1).size(), 2);
        EXPECT_EQUAL(Polygon(points1.begin(), points1.end()).size(), 2);

        EXPECT_EQUAL(Polygon(points2).size(), 5);
        EXPECT_EQUAL(Polygon(points2.begin(), points2.end()).size(), 5);
    }

    SECTION("contains North pole") {
        const std::vector<Polygon::value_type> points{{0, 90}, {0, 0}, {1, 0}, {1, 90}, {0, 90}};

        Polygon poly1(points);
        EXPECT(poly1.contains({0, 90}));
        EXPECT(poly1.contains({10, 90}));
        EXPECT_NOT(poly1.contains({0, -90}));
        EXPECT_NOT(poly1.contains({10, -90}));

        Polygon poly2(points, false);
        EXPECT(poly2.contains({0, 90}));
        EXPECT_NOT(poly2.contains({10, 90}));
        EXPECT_NOT(poly2.contains({0, -90}));
        EXPECT_NOT(poly2.contains({10, -90}));
    }

    SECTION("contains South pole") {
        const std::vector<Polygon::value_type> points{{0, -90}, {0, 0}, {1, 0}, {1, -90}, {0, -90}};

        Polygon poly1(points);
        EXPECT_NOT(poly1.contains({0, 90}));
        EXPECT_NOT(poly1.contains({10, 90}));
        EXPECT(poly1.contains({0, -90}));
        EXPECT(poly1.contains({10, -90}));

        Polygon poly2(points, false);
        EXPECT_NOT(poly2.contains({0, 90}));
        EXPECT_NOT(poly2.contains({10, 90}));
        EXPECT(poly2.contains({0, -90}));
        EXPECT_NOT(poly2.contains({10, -90}));
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
        EXPECT(poly3.contains({2 - 360, 1}));
        EXPECT(poly3.contains({2, 1}));
        EXPECT(poly3.contains({2 + 360, 1}));

        Polygon poly4({{-100, 18}, {21, 30}, {150, 50}, {260, 18}, {-100, 18}});
        EXPECT(poly4.contains({-10 - 360, 18}));
        EXPECT(poly4.contains({-10, 18}));
        EXPECT(poly4.contains({-10 + 360, 18}));
    }

    SECTION("MIR-566: winding number strict check of edges") {
        Polygon poly({{110, -34}, {90, -62}, {100, -59}, {110, -50}, {132, -40}, {110, -34}});
        EXPECT_NOT(poly.contains({90, -40}));
        EXPECT_NOT(poly.contains({90, -34}));
    }

    SECTION("Simple rectangular polygon") {
        double lonmin = 0;
        double lonmax = 360;
        double lonmid = 0.5 * (lonmin + lonmax);

        double latmax = 80;
        double latmin = 0;
        double latmid = 0.5 * (latmin + latmax);

        Polygon poly({{lonmin, latmax}, {lonmax, latmax}, {lonmax, latmin}, {lonmin, latmin}, {lonmin, latmax}});

        SECTION("Contains edges") {
            EXPECT(poly.contains({lonmin, latmax}));
            EXPECT(poly.contains({lonmid, latmax}));
            EXPECT(poly.contains({lonmax, latmax}));
            EXPECT(poly.contains({lonmax, latmid}));
            EXPECT(poly.contains({lonmax, latmin}));
            EXPECT(poly.contains({lonmid, latmin}));
            EXPECT(poly.contains({lonmin, latmin}));
            EXPECT(poly.contains({lonmin, latmid}));
        }

        SECTION("Contains in/outward of edges") {
            constexpr auto eps = 0.001;

            for (size_t i = 0; i <= 100; ++i) {
                auto lon = lonmin + static_cast<double>(i) * (lonmax - lonmin) / 99.;
                EXPECT(poly.contains({lon, latmin + eps}));
                EXPECT(poly.contains({lon, latmax - eps}));
                EXPECT_NOT(poly.contains({lon, latmin - eps}));
                EXPECT_NOT(poly.contains({lon, latmax + eps}));

                auto lat = latmin + static_cast<double>(i) * (latmax - latmin) / 99.;
                EXPECT(poly.contains({lonmin + eps, lat}));
                EXPECT(poly.contains({lonmax - eps, lat}));
                EXPECT_NOT(poly.contains({lonmin - eps, lat}));
                EXPECT_NOT(poly.contains({lonmax + eps, lat}));
            }
        }
    }

    SECTION("Degenerate polygon") {
        const std::vector<Polygon::value_type> points{{0, 0}, {2, 0}, {2, 0} /*duplicate*/, {0, 2}, {0, 0}};

        Polygon poly(points);

        for (const auto& p : points) {
            EXPECT(poly.contains(p));
        }

        for (const auto& p : std::vector<Polygon::value_type>{{2, 2}}) {
            EXPECT_NOT(poly.contains(p));
        }
    }

    SECTION("Self-intersecting polygon") {
        Polygon poly1({{-1, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}});

        EXPECT(poly1.contains({0, 0}));
        EXPECT(poly1.contains({-1, 0}));
        EXPECT(poly1.contains({1, 0}));
        EXPECT_NOT(poly1.contains({0, 1}));
        EXPECT_NOT(poly1.contains({0, -1}));

        Polygon poly2({{-1, -1}, {1, -1}, {-1, 1}, {1, 1}, {-1, -1}});

        EXPECT(poly2.contains({0, 0}));
        EXPECT_NOT(poly2.contains({-1, 0}));
        EXPECT_NOT(poly2.contains({1, 0}));
        EXPECT(poly2.contains({0, 1}));
        EXPECT(poly2.contains({0, -1}));
    }
}

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
