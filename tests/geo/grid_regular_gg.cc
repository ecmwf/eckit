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
#include "eckit/geo/grid/RegularGaussian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using RegularGaussian = grid::RegularGaussian;


CASE("sizes") {
    struct test_t {
        explicit test_t(size_t N) : N(N), size(4 * N * 2 * N) {}
        size_t N;
        size_t size;
    } tests[]{test_t{2}, test_t{3}, test_t{64}};

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f" + std::to_string(test.N)}})));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"type", "regular_gg"}, {"N", test.N}})));
        RegularGaussian grid3(test.N);

        EXPECT(grid1->size() == test.size);
        EXPECT(grid2->size() == test.size);
        EXPECT(grid3.size() == test.size);
    }
}


CASE("points") {
    RegularGaussian grid(1);

    const std::vector<PointLonLat> ref{
        {0., 35.264389683},  {90., 35.264389683},  {180., 35.264389683},  {270., 35.264389683},
        {0., -35.264389683}, {90., -35.264389683}, {180., -35.264389683}, {270., -35.264389683},
    };

    auto points = grid.to_points();

    EXPECT(points.size() == grid.size());
    ASSERT(points.size() == ref.size());

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


CASE("crop") {
    spec::Custom a({{"grid", "f2"}});
    std::unique_ptr<const Grid> grid1(GridFactory::build(a));
    auto n1 = grid1->size();

    EXPECT_EQUAL(n1, 32);

    a.set("south", 0.);
    std::unique_ptr<const Grid> grid2(GridFactory::build(a));
    auto n2 = grid2->size();

    EXPECT_EQUAL(n2, n1 / 2);

    spec::Custom b{{{"grid", "f2"}, {"west", -180}}};
    std::unique_ptr<const Grid> grid3(GridFactory::build(b));
    auto n3 = grid3->size();

    EXPECT_EQUAL(n3, n1);

    auto bbox3 = grid3->boundingBox();

    EXPECT(bbox3.periodic());

    bbox3 = {bbox3.north, bbox3.west, bbox3.south, 0.};

    EXPECT_NOT(bbox3.periodic());

    std::unique_ptr<const Grid> grid4(grid3->make_grid_cropped(bbox3));
    auto n4 = grid4->size();

    EXPECT_EQUAL(n4, 5 * 4);  // Ni * Nj

    b.set("east", -1.);
    std::unique_ptr<const Grid> grid5(GridFactory::build(b));
    auto n5 = grid5->size();

    EXPECT_EQUAL(n5, 4 * 4);  // Ni * Nj

    const std::vector<PointLonLat> ref{
        {-180., 59.444408289},  {-135., 59.444408289},  {-90., 59.444408289},  {-45., 59.444408289},
        {-180., 19.875719147},  {-135., 19.875719147},  {-90., 19.875719147},  {-45., 19.875719147},
        {-180., -19.875719147}, {-135., -19.875719147}, {-90., -19.875719147}, {-45., -19.875719147},
        {-180., -59.444408289}, {-135., -59.444408289}, {-90., -59.444408289}, {-45., -59.444408289},
    };

    auto points5 = grid5->to_points();

    EXPECT(points5.size() == grid5->size());
    ASSERT(points5.size() == ref.size());

    auto it = grid5->begin();
    for (size_t i = 0; i < points5.size(); ++i) {
        EXPECT(points_equal(ref[i], points5[i]));
        EXPECT(points_equal(ref[i], *it));
        ++it;
    }
    EXPECT(it == grid5->end());

    size_t i = 0;
    for (const auto& it : *grid5) {
        EXPECT(points_equal(ref[i++], it));
    }
    EXPECT_EQUAL(i, n5);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f3"}})));
    std::unique_ptr<const Grid> grid2(new RegularGaussian(3));

    EXPECT(*grid1 == *grid2);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
