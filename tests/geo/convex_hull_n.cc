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
#include <vector>

#include "eckit/geo/convexhull/ConvexHullN.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("ConvexHullN, N=2") {
    using Point2 = std::array<double, 2>;

    geo::convexhull::ConvexHullN ch(std::vector<Point2>{
        {1, 2},  // part of convex hull
        {3, 1},  //...
        {4, 4},
        {6, 5},
        {7, 2},  //...
        {2, 5},  //...
        {5, 7},
        {8, 3},  //...
        {6, 9},  //...
        {9, 6},  //...
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

        for (const auto& fr : std::vector<std::vector<size_t>>{
                 {3, 2},
                 {5, 1},
                 {5, 3},
                 {6, 1},
                 {6, 4},
                 {7, 2},
                 {7, 4},
             }) {
            ASSERT(fr.size() == 2);
            EXPECT(std::count_if(facets.begin(), facets.end(), [&fr](const auto& facet) {
                       ASSERT(facet.size() == 2);
                       return (fr[0] == facet[0] && fr[1] == facet[1]) || (fr[0] == facet[1] && fr[1] == facet[0]);
                   }) == 1);
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
