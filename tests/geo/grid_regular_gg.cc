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

#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/regular/RegularGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("RegularGaussian") {
    SECTION("sizes") {
        struct test_t {
            explicit test_t(size_t N) :
                N(N), size(4 * N * 2 * N) {}
            size_t N;
            size_t size;
        } tests[]{test_t{2}, test_t{3}, test_t{64}};

        for (const auto& test : tests) {
            std::unique_ptr<const Grid> grid1(
                GridFactory::build(spec::Custom({{"grid", "f" + std::to_string(test.N)}})));
            std::unique_ptr<const Grid> grid2(
                GridFactory::build(spec::Custom({{"type", "regular_gg"}, {"N", test.N}})));
            grid::regular::RegularGaussian grid3(test.N);

            EXPECT(grid1->size() == test.size);
            EXPECT(grid2->size() == test.size);
            EXPECT(grid3.size() == test.size);
        }
    }


    SECTION("points") {
        grid::regular::RegularGaussian grid(1);

        const std::vector<Point> ref{
            PointLonLat{0., 35.264389683},
            PointLonLat{90., 35.264389683},
            PointLonLat{180., 35.264389683},
            PointLonLat{270., 35.264389683},
            PointLonLat{0., -35.264389683},
            PointLonLat{90., -35.264389683},
            PointLonLat{180., -35.264389683},
            PointLonLat{270., -35.264389683},
        };

        auto points = grid.to_points();

        EXPECT(points.size() == grid.size());
        ASSERT(points.size() == ref.size());

        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(points[i], ref[i]));
        }
    }


    SECTION("crop") {
        spec::Custom spec({{"grid", "f2"}});
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
        auto n1 = grid1->size();

        EXPECT_EQUAL(n1, 32);

        spec.set("south", 0.);
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec));
        auto n2 = grid2->size();

        EXPECT_EQUAL(n2, n1 / 2);

        spec = spec::Custom{{{"grid", "f2"}, {"west", -180}}};
        std::unique_ptr<const Grid> grid3(GridFactory::build(spec));
        auto n3 = grid3->size();

        EXPECT_EQUAL(n3, n1);

        auto bbox3 = grid3->boundingBox();

        EXPECT(bbox3.isPeriodicWestEast());

        bbox3.east = 0.;

        EXPECT_NOT(bbox3.isPeriodicWestEast());

        std::unique_ptr<const Grid> grid4(grid3->grid_crop(bbox3));
        auto n4 = grid4->size();

        EXPECT_EQUAL(n4, 5 * 4);  // Ni * Nj

        spec.set("east", -1.);
        std::unique_ptr<const Grid> grid5(GridFactory::build(spec));
        auto n5 = grid5->size();

        EXPECT_EQUAL(n5, 4 * 4);  // Ni * Nj

        const std::vector<Point> ref{
            PointLonLat{-180., 59.444408289},
            PointLonLat{-135., 59.444408289},
            PointLonLat{-90., 59.444408289},
            PointLonLat{-45., 59.444408289},
            PointLonLat{-180., 19.875719147},
            PointLonLat{-135., 19.875719147},
            PointLonLat{-90., 19.875719147},
            PointLonLat{-45., 19.875719147},
            PointLonLat{-180., -19.875719147},
            PointLonLat{-135., -19.875719147},
            PointLonLat{-90., -19.875719147},
            PointLonLat{-45., -19.875719147},
            PointLonLat{-180., -59.444408289},
            PointLonLat{-135., -59.444408289},
            PointLonLat{-90., -59.444408289},
            PointLonLat{-45., -59.444408289},
        };

        auto points5 = grid5->to_points();

        EXPECT(points5.size() == grid5->size());
        ASSERT(points5.size() == ref.size());

        for (size_t i = 0; i < points5.size(); ++i) {
            EXPECT(points_equal(points5[i], ref[i]));
        }
    }


    SECTION("equals") {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f3"}})));
        std::unique_ptr<const Grid> grid2(new grid::regular::RegularGaussian(3));

        EXPECT(*grid1 == *grid2);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
