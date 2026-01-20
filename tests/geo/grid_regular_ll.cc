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

#include "eckit/geo/grid/RegularLL.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using RegularLL = grid::RegularLL;


CASE("global") {
    SECTION("1") {
        std::unique_ptr<Grid> grid(new RegularLL(spec::Custom{{{"grid", std::vector<double>{90, 90}}}}));

        EXPECT(grid->size() == 4 * 3);

        std::vector<PointLonLat> ref{{0., 90.},  {90., 90.},  {180., 90.},  {270., 90.},  //
                                     {0., 0.},   {90., 0.},   {180., 0.},   {270., 0.},   //
                                     {0., -90.}, {90., -90.}, {180., -90.}, {270., -90.}};

        EXPECT(points_equal(grid->first_point(), ref.front()));
        EXPECT(points_equal(grid->last_point(), ref.back()));

        const auto points = grid->to_points();

        ASSERT(points.size() == ref.size());
        EXPECT(points.size() == grid->size());

        auto q = ref.cbegin();
        for (const auto& p : points) {
            EXPECT(points_equal(p, *q++));
        }
    }


    SECTION("2") {
        std::unique_ptr<Grid> grid(new RegularLL(spec::Custom{{{"grid", std::vector<double>{1, 1}}}}));

        EXPECT(grid->size() == 360 * 181);
    }


    SECTION("3") {
        RegularLL grid(
            spec::Custom{{{"grid", std::vector<double>{2, 1}}, {"area", std::vector<double>{10, 1, 1, 10}}}});

        EXPECT(grid.size() == 5 * 10);
    }


    SECTION("4") {
        RegularLL grid({1., 1.}, {89.5, 0.5, -89.5, 359.5}, {0.5, 0.5});

        EXPECT(grid.nx() == 360);
        EXPECT(grid.ny() == 180);
        EXPECT(grid.size() == 360 * 180);
    }


    SECTION("5") {
        RegularLL grid({1., 1.}, {90., 0., -90, 360.}, {0.5, 0.5});

        EXPECT(grid.nx() == 360);
        EXPECT(grid.ny() == 180);
        EXPECT(grid.size() == 360 * 180);
    }
}


CASE("non-global") {
    SECTION("origin at (0, 0) (non-shifted)") {
        /*
         *  1
         *  0  .  .  .  .
         * -1
         *    -1  0  1  2
         */
        RegularLL grid({1, 2}, {1, -1, -1, 2});

        const std::vector<PointLonLat> ref{{-1., 0.}, {0., 0.}, {1., 0.}, {2., 0.}};

        auto points = grid.to_points();
        ASSERT(points.size() == ref.size());

        EXPECT(points.size() == grid.size());

        auto it = grid.begin();
        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(ref[i], points[i]));
            EXPECT(points_equal(ref[i], *it));
            ++it;
        }
        EXPECT(it == grid.end());

        size_t i = 0;
        for (const auto& it : grid) {
            EXPECT(points_equal(ref[i++], it));
        }
        EXPECT(i == grid.size());
    }


    SECTION("origin at (-1, -1) (shifted)") {
        /*
         *  1  .  .  .  .
         *  0
         * -1  .  .  .  .
         *    -1  0  1  2
         */
        RegularLL grid({1, 2}, {1, -1, -1, 2}, {-1, -1});

        const std::vector<PointLonLat> ref{
            {-1., 1.}, {0., 1.}, {1., 1.}, {2., 1.}, {-1., -1.}, {0., -1.}, {1., -1.}, {2., -1.},
        };

        auto points = grid.to_points();
        ASSERT(points.size() == ref.size());

        EXPECT(points.size() == grid.size());

        auto it = grid.begin();
        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(ref[i], points[i]));
            EXPECT(points_equal(ref[i], *it));
            ++it;
        }
        EXPECT(it == grid.end());

        size_t i = 0;
        for (const auto& it : grid) {
            EXPECT(points_equal(ref[i++], it));
        }
        EXPECT(i == grid.size());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
