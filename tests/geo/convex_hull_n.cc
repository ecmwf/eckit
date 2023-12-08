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


using namespace geo;

using Point2 = std::array<double, 2>;


CASE("PointLonLat normalisation") {
    std::vector<Point2> points{{
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
    }};

    std::vector<size_t> vertices_reference{0, 1, 4, 5, 7, 8, 9};

    convexhull::ConvexHullN ch(points);
    auto vertices = ch.list_vertices();

    EXPECT(vertices.size() == vertices_reference.size());
    for (const auto& vertex : vertices_reference) {
        EXPECT(std::find(vertices.begin(), vertices.end(), vertex) != vertices.end());
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
