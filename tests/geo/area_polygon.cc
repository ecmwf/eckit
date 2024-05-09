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


#include <vector>

#include "eckit/geo/Point2.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/polygon/LonLatPolygon.h"
#include "eckit/geo/polygon/Polygon.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Polygon") {
    using geo::polygon::Polygon;


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
    using Polygon = geo::polygon::LonLatPolygon;


    SECTION("Construction") {
        const std::vector<Polygon::value_type> points1{{0, 0}, {1, 1}, {2, 2}, {0, 0}};
        const std::vector<Polygon::value_type> points2{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2},
                                                       {1, 2}, {0, 2}, {0, 1}, {0, 0}};

        EXPECT_EQUAL(Polygon(points1).size(), 2);
        EXPECT_EQUAL(Polygon(points1.begin(), points1.end()).size(), 2);

        EXPECT_EQUAL(Polygon(points2).size(), 5);
        EXPECT_EQUAL(Polygon(points2.begin(), points2.end()).size(), 5);
    }


    SECTION("Contains North pole") {
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


    SECTION("Contains South pole") {
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


    SECTION("Contains South and North poles") {
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
        EXPECT(poly1.contains({362, 1}));
        EXPECT(poly1.contains({722, 1}));

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

        Polygon poly5({{-44.2299698513, 44.8732496764},
                       {-12.2849279262, 75.2545011911},
                       {72.2148603917, 76.7993105902},
                       {196.903572422, 71.1350094603},
                       {304.194105814, 52.8269579527},
                       {266.886210026, -17.7495991714},
                       {108.327652927, 34.8499103834},
                       {-96.2694736324, -17.4340627522},
                       {-99.8761719143, 7.28288763265},
                       {-44.2299698513, 44.8732496764}});
        for (double lon = -1, lat = 10; lat < 70; lat += 1) {
            EXPECT(poly5.contains({lon - 360, lat}));
            EXPECT(poly5.contains({lon, lat}));
            EXPECT(poly5.contains({lon + 360, lat}));
        }

        constexpr double eps   = 0.001;
        constexpr double globe = 360;
        Polygon poly6({{0 * globe, 4 + eps},
                       {1 * globe, 2 + eps},
                       {2 * globe, 0 + eps},
                       {3 * globe, -2 + eps},
                       {4 * globe, -4 + eps},
                       {4 * globe, -4 - eps},
                       {3 * globe, -2 - eps},
                       {2 * globe, 0 - eps},
                       {1 * globe, 2 - eps},
                       {0 * globe, 4 - eps},
                       {0 * globe, 4 + eps}});

        const std::vector<double> list_lons{-2. * globe, -globe, 0., globe, 2. * globe};
        const std::vector<double> list_lats1{4., 2., 0., -2.};
        const std::vector<double> list_lats2{5., 3., 1., -1., -3., -5.};
        for (double lon : list_lons) {
            for (double lat : list_lats1) {
                EXPECT(poly6.contains({lon + 180., lat - 1.}));
                EXPECT(poly6.contains({lon, lat}));
            }
            for (double lat : list_lats2) {
                EXPECT_NOT(poly6.contains({lon, lat}));
                EXPECT_NOT(poly6.contains({lon + 180., lat - 1.}));
            }
        }

        // HEALPix-like equator wedge in longitude
        Polygon poly(
            {{0, 1}, {0, 90}, {360, 90}, {360, 1}, {361, 0}, {360, -1}, {360, -90}, {0, -90}, {0, -1}, {1, 0}, {0, 1}});
        EXPECT(poly.contains({0, 0}));
        EXPECT(poly.contains({1, 0}));
        EXPECT(poly.contains({360, 0}));
        EXPECT(poly.contains({361, 0}));
        EXPECT(poly.contains({720, 0}));
        EXPECT(poly.contains({721, 0}));
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

        EXPECT(poly.contains({lonmin, latmax}));
        EXPECT(poly.contains({lonmid, latmax}));
        EXPECT(poly.contains({lonmax, latmax}));
        EXPECT(poly.contains({lonmax, latmid}));
        EXPECT(poly.contains({lonmax, latmin}));
        EXPECT(poly.contains({lonmid, latmin}));
        EXPECT(poly.contains({lonmin, latmin}));
        EXPECT(poly.contains({lonmin, latmid}));

        // Test contains in/outward of edges
        constexpr auto eps = 0.001;

        for (size_t i = 0; i <= 100; ++i) {
            const auto lon = lonmin + static_cast<double>(i) * (lonmax - lonmin) / 100.;
            EXPECT(poly.contains({lon, latmin + eps}));
            EXPECT(poly.contains({lon, latmax - eps}));
            EXPECT_NOT(poly.contains({lon, latmin - eps}));
            EXPECT_NOT(poly.contains({lon, latmax + eps}));

            const auto lat = latmin + static_cast<double>(i) * (latmax - latmin) / 100.;
            EXPECT(poly.contains({lonmin + eps, lat}));
            EXPECT(poly.contains({lonmax - eps, lat}));
            EXPECT(poly.contains({lonmin - eps, lat}));
            EXPECT(poly.contains({lonmax + eps, lat}));
        }

        // Test points at non-canonical coordinates
        // Default behavior throws
        EXPECT_THROWS_AS(poly.contains({lonmid, 91.}), BadValue);

        auto A = PointLonLat::make(lonmid + 360., latmid, lonmin);
        EXPECT(poly.contains({A.lon, A.lat}));

        auto B = PointLonLat::make(lonmid, 180. - latmid, lonmin);
        EXPECT(poly.contains({B.lon, B.lat}));
    }


    SECTION("Parallelogram") {
        const std::vector<Polygon::value_type> points{{0, 0}, {1, 1}, {2, 1}, {1, 0}, {0, 0}};
        Polygon poly(points);

        for (const auto& p : points) {
            EXPECT(poly.contains(p));
        }
        EXPECT_NOT(poly.contains({0, 1}));
        EXPECT_NOT(poly.contains({2, 0}));
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

        Polygon poly3({{-1, 89}, {1, 89}, {0, 90}, {181, 89}, {179, 89}, {0, 90}, {-1, 89}});
        EXPECT(poly3.size() == 7);

        const std::vector<double> list_lons{-720., -360., 0., 360., 720.};
        for (const auto& lon : list_lons) {
            EXPECT(poly3.contains({lon, 89.}));
            EXPECT(poly3.contains({lon + 180, 89.}));
            EXPECT_NOT(poly3.contains({lon + 90, 89.}));
            EXPECT_NOT(poly3.contains({lon + 270, 89.}));
        }
    }


    SECTION("Partitioning (includePoles=false)") {
        auto mid = [](double a, double b) { return (a + b) / 2.; };

        constexpr double lon[] = {0, 90, 180, 270, 360};
        constexpr double lat[] = {90, 0, -90};

        Polygon polys[] = {
            Polygon({{lon[0], lat[1]}, {lon[1], lat[1]}, {lon[1], lat[0]}, {lon[0], lat[0]}, {lon[0], lat[1]}}, false),
            Polygon({{lon[1], lat[1]}, {lon[2], lat[1]}, {lon[2], lat[0]}, {lon[1], lat[0]}, {lon[1], lat[1]}}, false),
            Polygon({{lon[2], lat[1]}, {lon[3], lat[1]}, {lon[3], lat[0]}, {lon[2], lat[0]}, {lon[2], lat[1]}}, false),
            Polygon({{lon[3], lat[1]}, {lon[4], lat[1]}, {lon[4], lat[0]}, {lon[3], lat[0]}, {lon[3], lat[1]}}, false),
            Polygon({{lon[0], lat[1]}, {lon[1], lat[1]}, {lon[1], lat[2]}, {lon[0], lat[2]}, {lon[0], lat[1]}}, false),
            Polygon({{lon[1], lat[1]}, {lon[2], lat[1]}, {lon[2], lat[2]}, {lon[1], lat[2]}, {lon[1], lat[1]}}, false),
            Polygon({{lon[2], lat[1]}, {lon[3], lat[1]}, {lon[3], lat[2]}, {lon[2], lat[2]}, {lon[2], lat[1]}}, false),
            Polygon({{lon[3], lat[1]}, {lon[4], lat[1]}, {lon[4], lat[2]}, {lon[3], lat[2]}, {lon[3], lat[1]}}, false)};


        std::vector<Polygon::value_type> points;
        const std::vector<double> list_lons{lon[0], mid(lon[0], lon[1]), lon[1], mid(lon[1], lon[2]),
                                            lon[2], mid(lon[2], lon[3]), lon[3], mid(lon[3], lon[4])};
        const std::vector<double> list_lats{lat[0], mid(lat[0], lat[1]), lat[1], mid(lat[1], lat[2]), lat[2]};

        for (double lon : list_lons) {
            for (double lat : list_lats) {
                points.emplace_back(lon, lat);
            }
        }

        std::vector<size_t> counts(points.size(), 0);
        for (size_t i = 0; i < points.size(); ++i) {
            for (const auto& poly : polys) {
                if (poly.contains(points[i])) {
                    ++counts[i];
                }
            }
        }

        for (size_t i = 0; i < counts.size(); i += list_lats.size() * 2) {
            EXPECT(counts[i + 0] == 2);
            EXPECT(counts[i + 1] == 2);
            EXPECT(counts[i + 2] == 4);
            EXPECT(counts[i + 3] == 2);
            EXPECT(counts[i + 4] == 2);

            EXPECT(counts[i + 5] == 1);
            EXPECT(counts[i + 6] == 1);
            EXPECT(counts[i + 7] == 2);
            EXPECT(counts[i + 8] == 1);
            EXPECT(counts[i + 9] == 1);
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
