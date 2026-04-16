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

#include "eckit/geo/grid/regular/RegularLL.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::regular::RegularLL;


CASE("global, non-shifted") {
    std::unique_ptr<Grid> a(new RegularLL(spec::Custom{{{"grid", std::vector<double>{90, 90}}}}));

    EXPECT(a->size() == 4 * 3);

    std::vector<PointLonLat> ref{{0., 90.},  {90., 90.},  {180., 90.},  {270., 90.},  //
                                 {0., 0.},   {90., 0.},   {180., 0.},   {270., 0.},   //
                                 {0., -90.}, {90., -90.}, {180., -90.}, {270., -90.}};

    EXPECT(points_equal(a->first_point(), ref.front()));
    EXPECT(points_equal(a->last_point(), ref.back()));

    const auto points = a->to_points();

    ASSERT(points.size() == ref.size());
    EXPECT(points.size() == a->size());

    auto q = ref.cbegin();
    for (const auto& p : points) {
        EXPECT(points_equal(p, *q++));
    }

    RegularLL b(spec::Custom{{{"grid", std::vector<double>{1, 1}}}});

    EXPECT(b.size() == 360 * 181);
    EXPECT(b.spec_str() == R"({"grid":[1,1]})");

    for (const double d : {
             7.,
             0.35,
             0.7,
             0.8,
             1.4,
             1.6,
         }) {
        const auto spec_ref = RegularLL({d, d}).spec_str();

        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom{{{"grid", std::vector<double>{d, d}}}}));

        EXPECT(spec_ref == grid1->spec_str());

        std::unique_ptr<const Grid> grid2(GridFactory::make_from_string(std::to_string(d) + "/" + std::to_string(d)));

        EXPECT(spec_ref == grid2->spec_str());
    }
}


CASE("global, shifted") {
    SECTION("shifted reference, 1/1") {
        for (const auto& grid : {
                 RegularLL({1., 1.}, {89.5, 0., -89.5, 360.}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {89.5, 0., -89.5, 359.5}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {89.5, 0.5, -89.5, 359.5}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {90., 0.5, -90., 360.}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {89.5, 0.5, -90., 360.5}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {90., 0.5, -89.5, 361.}, {0.5, 0.5}),
                 RegularLL({1., 1.}, {}, {0.5, 0.5}),
             }) {
            static const std::string spec_ref = R"({"grid":[1,1],"reference":[0.5,0.5]})";

            EXPECT(grid.nlon() == 360);
            EXPECT(grid.nlat() == 180);
            EXPECT(grid.size() == 360 * 180);
            EXPECT(grid.spec_str() == spec_ref);
        }
    }


    SECTION("shifted reference, 12/1") {
        for (const auto& grid : {
                 RegularLL({2., 1.}, {89.5, 0., -89.5, 360.}, {1., 0.5}),
                 RegularLL({2., 1.}, {89.5, 0., -89.5, 359.5}, {1., 0.5}),
                 RegularLL({2., 1.}, {89.5, 1., -89.5, 359.5}, {1., 0.5}),
                 RegularLL({2., 1.}, {90., 1., -90., 360.}, {1., 0.5}),
                 RegularLL({2., 1.}, {89.5, 1., -90., 360.5}, {1., 0.5}),
                 RegularLL({2., 1.}, {90., 1., -89.5, 361.}, {1., 0.5}),
                 RegularLL({2., 1.}, {}, {1., 0.5}),
             }) {
            static const std::string spec_ref = R"({"grid":[2,1],"reference":[1,0.5]})";

            EXPECT(grid.nlon() == 180);
            EXPECT(grid.nlat() == 180);
            EXPECT(grid.size() == 180 * 180);
            EXPECT(grid.spec_str() == spec_ref);
        }
    }


    SECTION("round-trip through spec (small)") {
        PointLonLat reference{45., 45.};
        RegularLL grid({90, 90}, {}, reference);

        std::vector<size_t> shape{2, 4};
        EXPECT(grid.shape() == shape);
        EXPECT(points_equal(grid.first_point(), reference));

        const Grid& grid_ref = grid;
        std::unique_ptr<const Grid> reconstructed(GridFactory::build(grid_ref.spec()));

        EXPECT(*reconstructed == grid);
    }


    SECTION("round-trip through spec (N96-like)") {
        PointLonLat reference{0.9375, 89.375};
        RegularLL grid({1.875, 1.25}, {}, reference);

        std::vector<size_t> shape{144, 192};
        EXPECT(grid.shape() == shape);
        EXPECT(points_equal(grid.first_point(), reference));

        const Grid& grid_ref = grid;
        std::unique_ptr<const Grid> reconstructed(GridFactory::build(grid_ref.spec()));

        EXPECT(*reconstructed == grid);
    }
}


CASE("non-global, non-shifted") {
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


CASE("non-global, shifted") {
    SECTION("shifted #1") {
        RegularLL b(spec::Custom{{{"grid", std::vector<double>{2, 1}}, {"area", std::vector<double>{10, 1, 1, 10}}}});

        EXPECT(b.size() == 5 * 10);
        EXPECT(b.spec_str() == R"({"area":[10,1,1,9],"grid":[2,1]})");
    }


    SECTION("shifted #2") {
        /*
         *  1
         *  0  .  .  .  .
         * -1
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


CASE("arakawa c-grids") {
    struct test_t {
        size_t N;
        std::vector<size_t> shape;
        PointLonLat inc;
    };

    auto increments = [](const Grid& g) -> PointLonLat {
        auto grid = g.spec().get_double_vector("grid");
        ASSERT(grid.size() == 2);
        return {grid[0], grid[1]};
    };

    auto reference = [](const Grid& g) -> PointLonLat {
        auto ref = g.spec().get_double_vector("reference");
        ASSERT(ref.size() == 2);
        return {ref[0], ref[1]};
    };

    for (const auto& test : {
             test_t{48, {72, 96}, {3.75, 2.5}},
             {96, {144, 192}, {1.875, 1.25}},
             {144, {216, 288}, {1.25, 0.833333333333333}},
             {216, {324, 432}, {0.833333333333333, 0.555555555555556}},
             {320, {480, 640}, {0.5625, 0.375}},
             {512, {768, 1024}, {0.3515625, 0.234375}},
             {768, {1152, 1536}, {0.234375, 0.15625}},
             {1280, {1920, 2560}, {0.140625, 0.09375}},
         }) {
        spec::Custom spec({{"type", "regular_ll_arakawa_c"}, {"N", test.N}});
        PointLonLat ref{test.inc.lon() / 2., test.inc.lat() / 2.};

        std::unique_ptr<const Grid> g(GridFactory::build(spec));

        EXPECT(points_equal(increments(*g), test.inc));
        EXPECT(points_equal(reference(*g), ref));
        EXPECT(g->shape() == test.shape);

        spec.set("arrangement", "T");
        std::unique_ptr<const Grid> t(GridFactory::build(spec));

        EXPECT(points_equal(increments(*t), test.inc));
        EXPECT(points_equal(reference(*g), ref));
        EXPECT(t->shape() == test.shape);

        spec.set("arrangement", "U");
        std::unique_ptr<const Grid> u(GridFactory::build(spec));

        EXPECT(points_equal(increments(*u), test.inc));
        EXPECT(points_equal(reference(*g), ref));
        EXPECT(u->shape() == test.shape);

        spec.set("arrangement", "V");
        std::unique_ptr<const Grid> v(GridFactory::build(spec));

        EXPECT(points_equal(increments(*v), test.inc));
        EXPECT(points_equal(reference(*g), ref));

        auto test_shape_v = test.shape;
        test_shape_v[0] += 1;
        EXPECT(v->shape() == test_shape_v);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
