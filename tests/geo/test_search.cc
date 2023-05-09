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

#include "grit/Search.h"


int main(int argc, const char* argv[]) {
    {
        std::vector<grit::SearchXYZ::Value> points{
            {{0, 0, 0}, 0},
        };

        grit::SearchXYZ search;
        search.build(points);

        std::cout << search.nearestNeighbour({0.1, 0, 0}) << std::endl;
        std::cout << search.nearestNeighbour({0.9, 0, 0}) << std::endl;

        search.insert({{1, 0, 0}, 1});

        std::cout << search.nearestNeighbour({0.1, 0, 0}) << std::endl;
        std::cout << search.nearestNeighbour({0.9, 0, 0}) << std::endl;
    }


    {
        std::vector<grit::SearchXY::Value> points{
            {{0, 0}, 0},
        };

        grit::SearchXY search;
        search.build(points);

        std::cout << search.nearestNeighbour({0.1, 0}) << std::endl;
        std::cout << search.nearestNeighbour({0.9, 0}) << std::endl;

        search.insert({{1, 0}, 1});

        std::cout << search.nearestNeighbour({0.1, 0}) << std::endl;
        std::cout << search.nearestNeighbour({0.9, 0}) << std::endl;
    }


    return 0;
}
