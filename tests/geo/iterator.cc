// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
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
        for (auto iter = grid->begin(); iter; ++iter) {
            EXPECT(points_equal(*iter, points[i]));
            EXPECT(points_equal(grid->point(i), points[i]));
            ++i;
        }
        EXPECT(i == expected_size);

        // test point in Grid
        size_t j = 0;
        for (const auto& point : *grid) {
            EXPECT(points_equal(point, points[j++]));
        }
        EXPECT(j == expected_size);
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


CASE("iterator: moving around") {
    using difference_type = Grid::Iterator::difference_type;

    for (const std::string spec :
         {"{grid: [30, 30]}", "{grid: [30, 30], order: i+-j-}", "{grid: O8}", "{grid: H4}", "{grid: H4n}"}) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
        const auto points = grid->to_points();
        const auto size   = static_cast<difference_type>(points.size());

        auto it = grid->begin();
        EXPECT(it != grid->end());

        // anywhere, in any direction (e.g. across rows)
        for (const difference_type k : {size - 1, size / 2, difference_type{1}, size / 3}) {
            auto at = grid->begin();
            EXPECT(at += k);
            EXPECT(at.index() == static_cast<size_t>(k));
            EXPECT(points_equal(*at, points[k]));

            EXPECT(--at);
            EXPECT(points_equal(*at, points[k - 1]));
        }

        // a copy stays where it was
        EXPECT(it += size / 2);
        auto copy = it;
        EXPECT(++it);
        EXPECT(points_equal(*copy, points[size / 2]));
        EXPECT(points_equal(*it, points[size / 2 + 1]));
        EXPECT(copy != it);

        // past either end is the end
        auto past = grid->begin();
        EXPECT_NOT(past += size);
        EXPECT_NOT(past);
        EXPECT(past == grid->end());

        auto before = grid->begin();
        EXPECT_NOT(--before);
        EXPECT(before == grid->end());
    }
}


CASE("iterator: standard algorithms") {
    using traits = std::iterator_traits<Grid::Iterator>;
    static_assert(std::is_same_v<traits::iterator_category, std::input_iterator_tag>);
    static_assert(std::is_same_v<traits::reference, decltype(*std::declval<Grid::Iterator>())>);

    for (const std::string spec : {"{grid: [30, 30]}", "{grid: [30, 30], order: i+-j-}", "{grid: O8}",
                                   "{grid: O8, order: i-j+}", "{grid: H4}", "{grid: H4n}"}) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
        const auto points = grid->to_points();
        const auto equal  = [](const Point& p, const Point& q) { return points_equal(p, q); };

        EXPECT(static_cast<size_t>(std::distance(grid->begin(), grid->end())) == points.size());

        const std::vector<Point> copied(grid->begin(), grid->end());
        EXPECT(std::equal(copied.begin(), copied.end(), points.begin(), points.end(), equal));

        auto it  = grid->begin();
        auto old = it++;
        EXPECT(old == grid->begin());
        EXPECT(it.index() == 1);
        EXPECT(points_equal(*old, points[0]));
        EXPECT(points_equal(*it--, points[1]));
        EXPECT(it == grid->begin());

        std::vector<Point> reversed;
        for (auto at = grid->end(); --at;) {
            reversed.push_back(*at);
        }
        EXPECT(std::equal(reversed.rbegin(), reversed.rend(), points.begin(), points.end(), equal));
    }
}


CASE("iterator: of different grids") {
    std::unique_ptr<const Grid> a(GridFactory::make_from_string("{grid: [30, 30]}"));
    std::unique_ptr<const Grid> b(GridFactory::make_from_string("{grid: [30, 30], order: i+j+}"));
    EXPECT(a->size() == b->size());

    EXPECT(a->begin() == a->begin());
    EXPECT(a->end() == a->end());
    EXPECT(a->begin() != b->begin());
    EXPECT(a->end() != b->end());
}


CASE("iterator: a reduced grid with rows without points") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: O32, area: [60, -10, 30, 40]}"));

    size_t i = 0;
    for (const auto& point : *grid) {
        EXPECT(points_equal(point, grid->point(i++)));
    }
    EXPECT(i == grid->size());
}


CASE("iterator: a grid without points") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{type: sh, truncation: 21}"));
    EXPECT_THROWS_AS(*grid->begin(), exception::GridError);
    EXPECT_THROWS_AS((void)grid->point(0), exception::GridError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
