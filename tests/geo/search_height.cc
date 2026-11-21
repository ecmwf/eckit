// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/grid/unstructured/UnstructuredLLH.h"
#include "eckit/geo/projection/LonLatHToXYZ.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


// Height-aware searches as in MeteoSwiss grid-process (grid_interp.py): grid points and points of interest (POIs) at
// heights above the figure, in a space distorted by height-scale-factor so neighbours at similar heights are nearer


constexpr double R = 6371229.;  // of the grid figure

const PointLonLat POI{7.44, 46.95};  // Bern
const double POI_HEIGHT = 520.;      // in a valley


// (lon, lat) at (east, north) metres of the POI
PointLonLat offset(double east, double north) {
    return {POI.lon() + util::RADIAN_TO_DEGREE * east / (R * std::cos(util::DEGREE_TO_RADIAN * POI.lat())),
            POI.lat() + util::RADIAN_TO_DEGREE * north / R};
}


// grid points (horizontal distance, height difference to the POI)
const std::vector<PointLonLat> LONLAT{
    offset(100., 0.),    // slope:  100 m,  200 m
    offset(-1000., 0.),  // valley: 1000 m, 0 m
    offset(0., 1500.),   // valley: 1500 m, 20 m
    offset(0., -50.),    // peak:   50 m,   1980 m
};
const std::vector<double> HEIGHT{720., 520., 540., 2500.};
const std::vector<double> HORIZONTAL{100., 1000., 1500., 50.};


grid::unstructured::UnstructuredLLH make_grid() {
    std::vector<double> lon;
    std::vector<double> lat;
    for (const auto& p : LONLAT) {
        lon.push_back(p.lon());
        lat.push_back(p.lat());
    }
    return {lon, lat, HEIGHT};
}


std::vector<size_t> indices(const Search::Neighbours& neighbours) {
    std::vector<size_t> result;
    for (const auto& n : neighbours) {
        result.push_back(n.index);
    }
    return result;
}


CASE("height-scale-factor: k nearest in a distorted space (interp_temp, z_fact)") {
    const auto grid = make_grid();
    const PointLonLatH poi{POI, POI_HEIGHT};

    SECTION("1 (default): nearest by Euclidean distance, the slope") {
        const Search search(grid);
        EXPECT(search.search_nn(poi).index == 0);
        EXPECT(indices(search.search_knn(poi, 4)) == std::vector<size_t>({0, 1, 2, 3}));
    }

    SECTION("25: nearest at similar heights, the valley") {
        const Search search(grid, spec::Custom{{"height-scale-factor", 25.}});
        EXPECT(search.search_nn(poi).index == 1);
        EXPECT(indices(search.search_knn(poi, 4)) == std::vector<size_t>({1, 2, 0, 3}));

        // as sqrt(horizontal^2 + (s dh)^2); horizontal distances are scaled by about (R + s h) / R, here 0.2%
        for (const auto& n : search.search_knn(poi, 4)) {
            const auto expected = std::hypot(HORIZONTAL[n.index], 25. * (HEIGHT[n.index] - POI_HEIGHT));
            EXPECT(std::abs(n.distance - expected) < 0.005 * expected);
        }

        // as points converted by llh-to-xyz of the same factor
        const projection::LonLatHToXYZ llh(FigureFactory::build(grid.figure()), 25.);
        EXPECT(search.search_nn(llh.fwd(poi)).index == 1);
    }

    SECTION("many POIs, configured search (as nn_lookup)") {
        const Search search(grid, spec::Custom{{"height-scale-factor", 25.}, {"search", "knn"}, {"search-k", 2}});

        // the second POI is on the peak
        const auto result = search.search(std::vector<Point>{poi, PointLonLatH{offset(0., -50.), 2400.}});
        ASSERT(result.size() == 2);
        EXPECT(indices(result[0]) == std::vector<size_t>({1, 2}));
        EXPECT(indices(result[1]) == std::vector<size_t>({3, 0}));
    }

    SECTION("k-d trees by factor") {
        EXPECT(Search::uid(grid) == Search::uid(grid, spec::Custom{{"height-scale-factor", 1.}}));
        EXPECT(Search::uid(grid) != Search::uid(grid, spec::Custom{{"height-scale-factor", 25.}}));
    }

    SECTION("(lon, lat) queries would be at height 0") {
        EXPECT_THROWS_AS(Search(grid).search_nn(POI), exception::SearchError);
    }
}


CASE("height-scale-factor 0: horizontal k nearest, then weighted (_grid_assignation, z_weight)") {
    const auto grid = make_grid();
    const Search search(grid, spec::Custom{{"height-scale-factor", 0.}});

    const auto neighbours = search.search_knn(PointLonLatH{POI, POI_HEIGHT}, 4);
    EXPECT(indices(neighbours) == std::vector<size_t>({3, 0, 1, 2}));

    // d = horizontal + z_weight * max(|dh| - deltah_min, 0)
    auto weighted = [](const Search::Neighbour& n) {
        return n.distance + 500. * std::max(std::abs(HEIGHT[n.index] - POI_HEIGHT) - 50., 0.);
    };

    const auto best = std::min_element(neighbours.begin(), neighbours.end(),
                                       [&](const auto& a, const auto& b) { return weighted(a) < weighted(b); });
    EXPECT(best->index == 1);
}


CASE("negative heights, depths below the figure") {
    // a point at three depths, and one nearby at the surface
    const std::vector<double> depth{100., 3000., 120., 0.};
    std::vector<double> height;
    for (auto d : depth) {
        height.push_back(-d);
    }

    const grid::unstructured::UnstructuredLLH grid({POI.lon(), POI.lon(), POI.lon(), LONLAT[0].lon()},
                                                   {POI.lat(), POI.lat(), POI.lat(), LONLAT[0].lat()}, height);

    // below the figure, along its normal
    const auto xyz = grid.to_xyz();
    EXPECT(types::is_approximately_equal(std::hypot(xyz[0][1], xyz[1][1], xyz[2][1]), R - 3000., 1e-6));

    const projection::LonLatHToXYZ llh(FigureFactory::build(grid.figure()));
    EXPECT(types::is_approximately_equal(llh.inv(PointXYZ{xyz[0][1], xyz[1][1], xyz[2][1]}).height(), -3000., 1e-6));

    // nearest at a similar depth, with depths weighing 1000 times
    const Search search(grid, spec::Custom{{"height-scale-factor", 1000.}});
    EXPECT(search.search_nn(PointLonLatH{POI, -110.}).index == 0);
    EXPECT(search.search_nn(PointLonLatH{POI, -2900.}).index == 1);
    EXPECT(indices(search.search_knn(PointLonLatH{POI, -115.}, 2)) == std::vector<size_t>({2, 0}));
}


CASE("height-scale-factor of points without heights") {
    EXPECT_THROWS_AS(Search(std::vector<PointXYZ>{{1., 0., 0.}}, spec::Custom{{"height-scale-factor", 25.}}),
                     exception::SearchError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
