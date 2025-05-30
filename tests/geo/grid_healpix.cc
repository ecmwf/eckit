/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to itr by virtue of its status as an intergovernmental organisation nor
 * does itr submit to any jurisdiction.
 */


#include <memory>

#include "eckit/geo/grid/HEALPix.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("gridspec") {
    for (size_t n : {1, 2}) {
        for (const std::string& suffix : {"", "n", "_nested", "r", "_ring"}) {
            const auto spec = "{grid: H" + std::to_string(n) + suffix + "}";
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
            EXPECT(grid->size() == 12 * n * n);
        }
    }

    spec::Custom spec1({{"grid", "H3"}});
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec1));

    EXPECT_EQUAL(grid1->size(), 108);
    EXPECT_EQUAL(grid1->spec_str(), R"({"grid":"H3"})");

    spec::Custom spec2({{"grid", "h2"}, {"order", "nested"}});
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec2));

    EXPECT_EQUAL(grid2->size(), 48);
    EXPECT_EQUAL(grid2->spec_str(), R"({"grid":"H2","order":"nested"})");

    spec::Custom spec3({{"grid", "h2"}, {"order", "ring"}});
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec3));

    EXPECT_EQUAL(grid3->size(), 48);
    EXPECT_EQUAL(grid3->spec_str(), R"({"grid":"H2"})");

    for (const std::string& name : {"h2N", "Hn2", "h2_nEsted"}) {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{"grid", name}}));
        EXPECT(*grid2 == *grid);
    }

    for (const std::string& name : {"H2", "h2r", "hR2", "h2_rinG"}) {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{"grid", name}}));
        EXPECT(*grid3 == *grid);
    }
}


CASE("sizes") {
    struct test_t {
        explicit test_t(size_t N) : N(N), size(12 * N * N) {}
        size_t N;
        size_t size;
    } tests[]{test_t{2}, test_t{3}, test_t{64}};

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "h" + std::to_string(test.N)}})));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"type", "HEALPix"}, {"Nside", test.N}})));
        grid::HEALPix grid3(test.N);

        EXPECT(grid1->size() == test.size);
        EXPECT(grid2->size() == test.size);
        EXPECT(grid3.size() == test.size);
    }
}


CASE("points") {
    std::unique_ptr<const Grid> ring(new grid::HEALPix(2));

    EXPECT(ring->order() == order::HEALPix::ring);


    std::unique_ptr<const Grid> nested(new grid::HEALPix(2, order::HEALPix::nested));

    EXPECT(nested->order() == order::HEALPix::nested);

    // reference coordinates in ring order()
    const std::vector<PointLonLat> ref{
        {45., 66.443535691},
        {135., 66.443535691},
        {225., 66.443535691},
        {315., 66.443535691},
        {22.5, 41.810314896},
        {67.5, 41.810314896},
        {112.5, 41.810314896},
        {157.5, 41.810314896},
        {202.5, 41.810314896},
        {247.5, 41.810314896},
        {292.5, 41.810314896},
        {337.5, 41.810314896},
        {0., 19.471220634},
        {45., 19.471220634},
        {90., 19.471220634},
        {135., 19.471220634},
        {180., 19.471220634},
        {225., 19.471220634},
        {270., 19.471220634},
        {315., 19.471220634},
        {22.5, 0.},
        {67.5, 0.},
        {112.5, 0.},
        {157.5, 0.},
        {202.5, 0.},
        {247.5, 0.},
        {292.5, 0.},
        {337.5, 0.},
        {0., -19.471220634},
        {45., -19.471220634},
        {90., -19.471220634},
        {135., -19.471220634},
        {180., -19.471220634},
        {225., -19.471220634},
        {270., -19.471220634},
        {315., -19.471220634},
        {22.5, -41.810314896},
        {67.5, -41.810314896},
        {112.5, -41.810314896},
        {157.5, -41.810314896},
        {202.5, -41.810314896},
        {247.5, -41.810314896},
        {292.5, -41.810314896},
        {337.5, -41.810314896},
        {45., -66.443535691},
        {135., -66.443535691},
        {225., -66.443535691},
        {315., -66.443535691},
    };


    auto points_r = ring->to_points();

    EXPECT(points_r.size() == ring->size());
    ASSERT(points_r.size() == ref.size());

    auto itr = ring->begin();
    for (size_t i = 0; i < points_r.size(); ++i) {
        EXPECT(points_equal(ref[i], points_r[i]));
        EXPECT(points_equal(ref[i], *itr));
        ++itr;
    }

    EXPECT(itr == ring->end());

    size_t i = 0;
    for (const auto& it : *ring) {
        EXPECT(points_equal(ref[i++], it));
    }

    EXPECT(i == ring->size());


    auto ren      = nested->reorder(order::HEALPix::ring);
    auto points_n = nested->to_points();

    EXPECT(points_n.size() == nested->size());
    ASSERT(points_n.size() == ref.size());

    auto it = nested->begin();
    for (size_t i = 0; i < points_n.size(); ++i) {
        EXPECT(points_equal(ref[ren.at(i)], points_n[i]));
        EXPECT(points_equal(ref[ren.at(i)], *it));
        ++it;
    }

    EXPECT(it == nested->end());

    size_t j = 0;
    for (const auto& it : *nested) {
        EXPECT(points_equal(ref[ren.at(j++)], it));
    }

    EXPECT(i == nested->size());
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "h2"}})));
    std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("{type: HEALPix, Nside: 2}"));
    std::unique_ptr<const Grid> grid3(new grid::HEALPix(2));

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == *grid1);

    EXPECT(grid1->order() == order::HEALPix::ring);

    std::unique_ptr<const Grid> grid4(GridFactory::build(spec::Custom({{"grid", "h2"}, {"order", "nested"}})));
    std::unique_ptr<const Grid> grid5(GridFactory::make_from_string("{type: HEALPix, Nside: 2, order: nested}"));
    std::unique_ptr<const Grid> grid6(new grid::HEALPix(2, order::HEALPix::nested));

    EXPECT(*grid4 != *grid1);

    EXPECT(*grid4 == *grid5);
    EXPECT(*grid5 == *grid6);
    EXPECT(*grid6 == *grid4);

    EXPECT(grid4->order() == order::HEALPix::nested);
}


CASE("wrong spec") {
    EXPECT_THROWS_AS(auto* ignore = GridFactory::make_from_string("{grid:h0}"), exception::SpecError);
    EXPECT_THROWS_AS(auto* ignore = GridFactory::make_from_string("{grid:h3, order:nested}"), exception::OrderError);
    EXPECT_THROWS_AS(auto* ignore = GridFactory::make_from_string("{grid:h3, order:?}"), exception::OrderError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
