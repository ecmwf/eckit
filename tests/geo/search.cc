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

#include "eckit/geo/Search.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Search2") {
    std::vector<Search2::Value> points{
        {{0., 0.}, 0},
    };

    Search2 search;
    search.build(points);

    auto a = search.nearestNeighbour({0.1, 0.});
    EXPECT_EQUAL(a.payload(), 0);

    auto b = search.nearestNeighbour({0.9, 0.});
    EXPECT_EQUAL(b.payload(), 0);

    search.insert({{1., 0.}, 1});

    auto c = search.nearestNeighbour({0.1, 0.});
    EXPECT_EQUAL(c.payload(), 0);

    auto d = search.nearestNeighbour({0.9, 0.});
    EXPECT_EQUAL(d.payload(), 1);
}


CASE("Search3") {
    std::vector<Search3::Value> points{
        {{0., 0., 0.}, 0},
    };

    Search3 search;
    search.build(points);

    auto a = search.nearestNeighbour({0.1, 0., 0.});
    EXPECT_EQUAL(a.payload(), 0);

    auto b = search.nearestNeighbour({0.9, 0., 0.});
    EXPECT_EQUAL(b.payload(), 0);

    search.insert({{1., 0., 0.}, 1});

    auto c = search.nearestNeighbour({0.1, 0., 0.});
    EXPECT_EQUAL(c.payload(), 0);

    auto d = search.nearestNeighbour({0.9, 0., 0.});
    EXPECT_EQUAL(d.payload(), 1);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
