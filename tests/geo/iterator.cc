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


#include <memory>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Iterator.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("iterator") {
    auto test = [](const std::string& gridspec, size_t expected_size) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(gridspec));
        ASSERT(grid);
        EXPECT(grid->size() == expected_size);

        // test Grid::to_points
        auto points = grid->to_points();
        EXPECT(points.size() == expected_size);

        size_t i = 0;
        for (auto iter = grid->begin(), end = grid->end(); iter; ++iter) {
            EXPECT(points_equal(*iter, points[i++]));
        }
        EXPECT(i == expected_size);

        // test point in Grid
        size_t j = 0;
        for (const auto& point : *grid) {
            EXPECT(points_equal(point, points[j++]));
        }
        EXPECT(j == expected_size);

        // test NextIterator
        decltype(points) next_points;
        Point point = PointLonLat{};
        for (auto point : *grid) {
            next_points.emplace_back(point);
        }

        EXPECT(next_points.size() == expected_size);

        size_t k = 0;
        for (const auto& next_point : next_points) {
            EXPECT(points_equal(next_point, points[k++]));
        }
    };


    SECTION("HEALPix") {
        for (size_t Nside = 1; Nside <= 3; ++Nside) {
            test("{grid: h" + std::to_string(Nside) + "}", 12 * Nside * Nside);
        }
    }


    SECTION("Reduced Gaussian") {
        for (size_t N = 1; N <= 3; ++N) {
            test("{grid: O" + std::to_string(N) + "}", 4 * N * N + 36 * N);
        }
    }


    SECTION("Regular Gaussian") {
        for (size_t N = 1; N <= 3; ++N) {
            test("{grid: F" + std::to_string(N) + "}", 8 * N * N);
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
