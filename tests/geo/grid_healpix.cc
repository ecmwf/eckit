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
    spec::Custom spec({{"grid", "h2"}});
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
    auto n1 = grid1->size();

    EXPECT_EQUAL(n1, 48);
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

    EXPECT(ring->ordering() == Ordering::healpix_ring);


    std::unique_ptr<const Grid> nested(new grid::HEALPix(2, Ordering::healpix_nested));

    EXPECT(nested->ordering() == Ordering::healpix_nested);

    // reference coordinates in ring ordering
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


    auto ren      = nested->reorder(Ordering::healpix_ring);
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

    EXPECT(grid1->ordering() == Ordering::healpix_ring);

    std::unique_ptr<const Grid> grid4(GridFactory::build(spec::Custom({{"grid", "h2"}, {"ordering", "nested"}})));
    std::unique_ptr<const Grid> grid5(GridFactory::make_from_string("{type: HEALPix, Nside: 2, ordering: nested}"));
    std::unique_ptr<const Grid> grid6(new grid::HEALPix(2, Ordering::healpix_nested));

    EXPECT(*grid4 != *grid1);

    EXPECT(*grid4 == *grid5);
    EXPECT(*grid5 == *grid6);
    EXPECT(*grid6 == *grid4);

    EXPECT(grid4->ordering() == Ordering::healpix_nested);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
