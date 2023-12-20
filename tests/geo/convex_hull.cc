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


#include <algorithm>
#include <array>
#include <memory>
#include <set>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/convexhull/ConvexHullN.h"
#include "eckit/geo/util/Qhull.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("Qhull errors/exceptions") {
    using geo::util::Qhull;


    SECTION("input") {
        for (size_t N : {2, 3, 4}) {
            Qhull::coord_t coord((N + 1) * N + 1, 1.);
            ASSERT(coord.size() % N != 0);
            EXPECT_THROWS_AS(Qhull(N, coord, "Qt"), AssertionFailed);  // 0 < N && coord.size() % N == 0
        }
    }


    SECTION("qhull") {
        struct {
            const int errorCode;
            const std::string what;
            const std::string command;
            const size_t N;
            const Qhull::coord_t coord;
        } static const tests[] = {
            {6050, "QH6050 qhull error: dimension 1 must be > 1", "Qt", 1, {1, 1}},
            {6154, "QH6154 Qhull precision error: Initial simplex is flat", "Qt", 2, {1, 1, 2, 2, 3, 3}},
            {6214, "QH6214 qhull input error: not enough points", "Qt", 2, {1, 1}},
            {6412, "QH6412 qhull input error (qh_initqhull_globals)", "Qt", 2, {}},
            {6421, "QH6421 qhull internal error (qh_maxsimplex)", "Qt", 2, {1, 1, 1, 1, 1, 1}},
        };

        for (const auto& test : tests) {
            try {
                Qhull(test.N, test.coord, test.command);
                EXPECT(false);
            }
            catch (const Qhull::Exception& e) {
                EXPECT_EQUAL(test.errorCode, e.errorCode);
                EXPECT_EQUAL(test.what, std::string(e.what(), test.what.length()));
            }
        }
    }
}


CASE("ConvexHullN, N=2") {
    using Point2 = std::array<double, 2>;

    // Polygon (vertices 0, 1, 4, 5, 7, 8, 9) containing point indices 2, 3, 6
    geo::convexhull::ConvexHullN ch(std::vector<Point2>{
        {1, 2},
        {3, 1},
        {4, 4},
        {6, 5},
        {7, 2},
        {2, 5},
        {5, 7},
        {8, 3},
        {6, 9},
        {9, 6},
    });


    SECTION("vertices") {
        const auto vertices = ch.list_vertices();
        EXPECT(vertices.size() == 7);

        for (size_t vertex : {0, 1, 4, 5, 7, 8, 9}) {
            EXPECT(std::find(vertices.begin(), vertices.end(), vertex) != vertices.end());
        }

        for (size_t vertex : {2, 3, 6}) {
            EXPECT(std::find(vertices.begin(), vertices.end(), vertex) == vertices.end());
        }
    }


    SECTION("facets") {
        const auto facets = ch.list_facets();
        EXPECT(facets.size() == 7);

        for (const auto& fr : {std::vector<size_t>{8, 9}, {5, 0}, {5, 8}, {1, 0}, {1, 4}, {7, 9}, {7, 4}}) {
            EXPECT(std::count_if(facets.begin(), facets.end(), [&fr](const auto& facet) {
                       ASSERT(facet.size() == 2);
                       return (fr[0] == facet[0] && fr[1] == facet[1]) || (fr[0] == facet[1] && fr[1] == facet[0]);
                   }) == 1);
        }

        for (size_t vertex : {2, 3, 6}) {
            for (const auto& facet : facets) {
                EXPECT(std::find(facet.begin(), facet.end(), vertex) == facet.end());
            }
        }
    }


    SECTION("factory") {
        // same as above
        std::unique_ptr<geo::ConvexHull> ch2(geo::ConvexHullFactory::instance()
                                                 .get("convex-hull-n-2")
                                                 .create(std::vector<std::vector<double>>{
                                                     {1, 2},
                                                     {3, 1},
                                                     {4, 4},
                                                     {6, 5},
                                                     {7, 2},
                                                     {2, 5},
                                                     {5, 7},
                                                     {8, 3},
                                                     {6, 9},
                                                     {9, 6},
                                                 }));

        auto vertex_set = [](const std::vector<size_t>& list) { return std::set<size_t>{list.cbegin(), list.cend()}; };
        EXPECT(vertex_set(ch.list_vertices()) == vertex_set(ch2->list_vertices()));

        // wrong dimensions
        EXPECT_THROWS_AS(geo::ConvexHullFactory::instance()
                             .get("convex-hull-n-3")
                             .create(std::vector<std::vector<double>>{
                                 {1, 2},
                                 {3, 1},
                                 {4, 4},
                             }),
                         AssertionFailed);
    }
}


CASE("ConvexHullN, N=3") {
    using Point3 = std::array<double, 3>;

    // Tetrahedron (vertices 0, 1, 2, 3) containing point index 4
    geo::convexhull::ConvexHullN ch(std::vector<Point3>{
        {0, 0, 1},
        {1, 0, -1},
        {-1, 1, -1},
        {-1, -1, -1},
        {0, 0, 0},
    });


    SECTION("vertices") {
        const auto vertices = ch.list_vertices();
        EXPECT(vertices.size() == 4);

        for (size_t vertex = 0; vertex < 4; ++vertex) {
            EXPECT(std::find(vertices.begin(), vertices.end(), vertex) != vertices.end());
        }

        EXPECT(std::find(vertices.begin(), vertices.end(), 4) == vertices.end());
    }


    SECTION("facets/triangles") {
        const auto facets = ch.list_facets();
        EXPECT(facets.size() == 4);

        const auto triangles = ch.list_triangles();
        EXPECT(triangles.size() == 4);

        for (const auto& fr : {
                 std::vector<size_t>{0, 1, 2},
                 {0, 1, 3},
                 {0, 2, 3},
                 {1, 2, 3},
             }) {
            EXPECT(std::count_if(facets.begin(), facets.end(), [&fr](const auto& facet) {
                       ASSERT(facet.size() == 3);
                       return std::count(fr.begin(), fr.end(), facet[0]) == 1 &&
                              std::count(fr.begin(), fr.end(), facet[1]) == 1 &&
                              std::count(fr.begin(), fr.end(), facet[2]) == 1;
                   }) == 1);

            EXPECT(std::count_if(triangles.begin(), triangles.end(), [&fr](const auto& tri) {
                       return std::count(fr.begin(), fr.end(), tri[0]) == 1 &&
                              std::count(fr.begin(), fr.end(), tri[1]) == 1 &&
                              std::count(fr.begin(), fr.end(), tri[2]) == 1;
                   }) == 1);
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
