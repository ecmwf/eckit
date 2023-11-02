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


#include <iostream>
#include <vector>

#include "eckit/geo/Search.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


CASE("") {
    std::vector<Search3::Value> points{
        {{0, 0, 0}, 0},
    };

    Search3 search;
    search.build(points);

    std::cout << search.nearestNeighbour({0.1, 0, 0}) << std::endl;
    std::cout << search.nearestNeighbour({0.9, 0, 0}) << std::endl;

    search.insert({{1, 0, 0}, 1});

    std::cout << search.nearestNeighbour({0.1, 0, 0}) << std::endl;
    std::cout << search.nearestNeighbour({0.9, 0, 0}) << std::endl;
}


CASE("") {
    std::vector<Search2::Value> points{
        {{0, 0}, 0},
    };

    Search2 search;
    search.build(points);

    std::cout << search.nearestNeighbour({0.1, 0}) << std::endl;
    std::cout << search.nearestNeighbour({0.9, 0}) << std::endl;

    search.insert({{1, 0}, 1});

    std::cout << search.nearestNeighbour({0.1, 0}) << std::endl;
    std::cout << search.nearestNeighbour({0.9, 0}) << std::endl;
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
