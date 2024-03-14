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
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("ReducedGaussianOctahedral") {
    SECTION("gridspec") {
        // different ways to instantiate the same grid (O2)
        for (auto spec : {
                 spec::Custom({{"grid", "o2"}}),
                 spec::Custom({{"N", 2}}),
                 spec::Custom({{"pl", pl_type{20, 24, 24, 20}}}),
             }) {
            std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
            auto n1 = grid1->size();

            EXPECT_EQUAL(n1, 88);

            spec.set("south", 0);
            std::unique_ptr<const Grid> grid2(GridFactory::build(spec));
            auto n2 = grid2->size();

            EXPECT_EQUAL(n2, n1 / 2);
        }
    }


    SECTION("sizes") {
        struct test_t {
            explicit test_t(size_t N) :
                N(N), size(4 * N * (N + 9)) {}
            size_t N;
            size_t size;
        } tests[]{test_t{2}, test_t{3}, test_t{64}};

        for (const auto& test : tests) {
            std::unique_ptr<const Grid> grid1(
                GridFactory::build(spec::Custom({{"grid", "o" + std::to_string(test.N)}})));
            std::unique_ptr<const Grid> grid2(
                GridFactory::build(spec::Custom({{"type", "reduced_gg"}, {"N", test.N}})));
            grid::reduced::ReducedGaussian grid3(test.N);

            EXPECT(grid1->size() == test.size);
            EXPECT(grid2->size() == test.size);
            EXPECT(grid3.size() == test.size);
        }
    }


    SECTION("points") {
        grid::reduced::ReducedGaussian grid(1);

        const std::vector<Point> ref{
            PointLonLat{0., 35.264389683},    PointLonLat{18., 35.264389683},   PointLonLat{36., 35.264389683},
            PointLonLat{54., 35.264389683},   PointLonLat{72., 35.264389683},   PointLonLat{90., 35.264389683},
            PointLonLat{108., 35.264389683},  PointLonLat{126., 35.264389683},  PointLonLat{144., 35.264389683},
            PointLonLat{162., 35.264389683},  PointLonLat{180., 35.264389683},  PointLonLat{198., 35.264389683},
            PointLonLat{216., 35.264389683},  PointLonLat{234., 35.264389683},  PointLonLat{252., 35.264389683},
            PointLonLat{270., 35.264389683},  PointLonLat{288., 35.264389683},  PointLonLat{306., 35.264389683},
            PointLonLat{324., 35.264389683},  PointLonLat{342., 35.264389683},  PointLonLat{0., -35.264389683},
            PointLonLat{18., -35.264389683},  PointLonLat{36., -35.264389683},  PointLonLat{54., -35.264389683},
            PointLonLat{72., -35.264389683},  PointLonLat{90., -35.264389683},  PointLonLat{108., -35.264389683},
            PointLonLat{126., -35.264389683}, PointLonLat{144., -35.264389683}, PointLonLat{162., -35.264389683},
            PointLonLat{180., -35.264389683}, PointLonLat{198., -35.264389683}, PointLonLat{216., -35.264389683},
            PointLonLat{234., -35.264389683}, PointLonLat{252., -35.264389683}, PointLonLat{270., -35.264389683},
            PointLonLat{288., -35.264389683}, PointLonLat{306., -35.264389683}, PointLonLat{324., -35.264389683},
            PointLonLat{342., -35.264389683},
        };

        auto points = grid.to_points();

        EXPECT(points.size() == grid.size());
        ASSERT(points.size() == ref.size());

        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(points[i], ref[i]));
        }
    }


    SECTION("crop") {
        spec::Custom spec({{"grid", "o2"}});
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
        auto n1 = grid1->size();

        EXPECT_EQUAL(n1, 88);

        spec.set("south", 0.);
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec));
        auto n2 = grid2->size();

        EXPECT_EQUAL(n2, n1 / 2);

        spec = spec::Custom{{{"grid", "o2"}, {"west", -180}}};
        std::unique_ptr<const Grid> grid3(GridFactory::build(spec));
        auto n3 = grid3->size();

        EXPECT_EQUAL(n3, n1);

        EXPECT(grid3->boundingBox().isPeriodicWestEast());

        // (exclude Greenwhich meridian)
        std::unique_ptr<const Grid> grid4(grid3->make_grid_cropped(area::BoundingBox(90., -180., 0., -1.)));
        auto n4 = grid4->size();

        EXPECT_EQUAL(n4, n3 / 4);

#if 0
        const std::vector<Point> ref{
            ...
        };

        auto points4 = grid4->to_points();

        EXPECT(points4.size() == grid4->size());
        ASSERT(points4.size() == ref.size());

        for (size_t i = 0; i < points4.size(); ++i) {
            EXPECT(points_equal(points4[i], ref[i]));
        }
#endif
    }


    SECTION("equals") {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "o3"}})));
        std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("N: 3"));
        std::unique_ptr<const Grid> grid3(new grid::reduced::ReducedGaussian(3));
        std::unique_ptr<const Grid> grid4(new grid::reduced::ReducedGaussian(pl_type{20, 24, 28, 28, 24, 20}));

        EXPECT(*grid1 == *grid2);
        EXPECT(*grid2 == *grid3);
        EXPECT(*grid3 == *grid4);
        EXPECT(*grid4 == *grid1);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
