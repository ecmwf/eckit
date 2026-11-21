// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include "eckit/eckit.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/geo/grid/unstructured/UnstructuredLLH.h"
#include "eckit/geo/projection/LonLatHToXYZ.h"
#include "eckit/geo/projection/ToFromXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


// searches of many points, on threads if OpenMP (see CMakeLists.txt), by k-d tree loaders in memory and in a cache file


using projection::ToFromXYZ;


std::vector<PointLonLat> random_lonlat(size_t n, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> lon(-180., 180.);
    std::uniform_real_distribution<double> sinlat(-1., 1.);

    std::vector<PointLonLat> points;
    points.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        points.emplace_back(lon(gen), std::asin(sinlat(gen)) * 180. / M_PI);
    }
    return points;
}


std::vector<PointXYZ> on_unit_sphere(const std::vector<PointLonLat>& points) {
    std::vector<PointXYZ> xyz;
    xyz.reserve(points.size());
    for (const auto& p : points) {
        xyz.emplace_back(ToFromXYZ::to_xyz(1., p));
    }
    return xyz;
}


// the CSR result is the neighbours of each query, by row
void expect_csr(const Search::CSR& csr, const std::vector<Search::Neighbours>& expected, size_t cols,
                Search::ColumnOrder order = Search::ColumnOrder::DISTANCE) {
    EXPECT(csr.rows() == expected.size());
    EXPECT(csr.cols == cols);
    EXPECT(csr.offsets.front() == 0);
    EXPECT(static_cast<size_t>(csr.offsets.back()) == csr.size());
    EXPECT(csr.distances.size() == csr.size());

    for (size_t i = 0; i < csr.rows(); ++i) {
        auto e = expected[i];
        if (order == Search::ColumnOrder::INDEX) {
            std::sort(e.begin(), e.end(), [](const auto& a, const auto& b) { return a.index < b.index; });
        }

        const auto begin = static_cast<size_t>(csr.offsets[i]);
        EXPECT(static_cast<size_t>(csr.offsets[i + 1]) - begin == e.size());
        for (size_t j = 0; j < e.size(); ++j) {
            EXPECT(static_cast<size_t>(csr.indices[begin + j]) == e[j].index);
            EXPECT(csr.distances[begin + j] == e[j].distance);
        }
    }
}


CASE("Search::csr_search: as search of each point") {
    EXPECT(LibEcKitGeo::searchNumThreads() == (eckit_HAVE_OMP ? 4 : 1));

    const auto points  = on_unit_sphere(random_lonlat(2000, 1));
    const auto queries = on_unit_sphere(random_lonlat(500, 2));

    const Search search(points, spec::Custom{{"R", 1.}});

    for (const auto& spec : {spec::Custom{{"search", "nn"}}, spec::Custom{{"search", "knn"}, {"search-k", 7}},
                             spec::Custom{{"search", "radius"}, {"search-radius", 0.05}},
                             spec::Custom{{"search", "knn_or_radius"}, {"search-k", 2}, {"search-radius", 0.05}}}) {
        const Search s(search, spec);

        std::vector<Search::Neighbours> expected;
        for (const auto& q : queries) {
            expected.emplace_back(s.search(q));
        }

        expect_csr(s.csr_search(queries), expected, points.size());
        expect_csr(s.csr_search(queries, Search::ColumnOrder::INDEX), expected, points.size(),
                   Search::ColumnOrder::INDEX);

        // as search(points), on threads
        EXPECT(s.search(std::vector<Point>(queries.begin(), queries.end())) == expected);
    }

    SECTION("query order: in the order given, the same results") {
        const Search knn(search, spec::Custom{{"search", "knn"}, {"search-k", 7}});
        const Search input(search, spec::Custom{{"search", "knn"}, {"search-k", 7}, {"search-query-order", "input"}});
        const auto a = knn.csr_search(queries);
        const auto b = input.csr_search(queries);
        EXPECT(a.indices == b.indices);
        EXPECT(a.distances == b.distances);

        EXPECT_THROWS_AS(Search(search, spec::Custom{{"search-query-order", "hilbert"}}), exception::SearchError);
    }

    SECTION("rows without neighbours") {
        const Search radius(search, spec::Custom{{"search", "radius"}, {"search-radius", 0.}});
        const auto csr = radius.csr_search(queries);
        EXPECT(csr.rows() == queries.size());
        EXPECT(csr.size() == 0);
        EXPECT(std::all_of(csr.offsets.begin(), csr.offsets.end(), [](int o) { return o == 0; }));
    }

    SECTION("no queries") {
        const auto csr = search.csr_search(std::vector<PointXYZ>{});
        EXPECT(csr.rows() == 0);
        EXPECT(csr.cols == points.size());
        EXPECT(csr.offsets == std::vector<int>{0});
    }

    SECTION("queries on points, at distance 0") {
        const std::vector<PointXYZ> on{points[3], points[1000]};
        const auto csr = search.csr_search(on);
        EXPECT(csr.indices == std::vector<int>({3, 1000}));
        EXPECT(csr.distances == std::vector<double>({0., 0.}));
    }
}


CASE("Search::csr_search: (lon, lat) queries") {
    const auto lonlat = random_lonlat(1000, 5);
    std::vector<double> lon;
    std::vector<double> lat;
    for (const auto& p : lonlat) {
        lon.push_back(p.lon());
        lat.push_back(p.lat());
    }

    const grid::Unstructured grid(lon, lat);
    const Search search(grid, spec::Custom{{"search", "knn"}, {"search-k", 3}});

    const auto queries = random_lonlat(100, 6);
    std::vector<PointXYZ> xyz;
    for (const auto& q : queries) {
        xyz.emplace_back(ToFromXYZ::to_xyz(grid.figure().a(), grid.figure().b(), q));
    }

    const auto a = search.csr_search(queries);
    const auto b = search.csr_search(xyz);
    EXPECT(a.offsets == b.offsets);
    EXPECT(a.indices == b.indices);
    EXPECT(a.distances == b.distances);

    // (lon, lat, height) queries require points with heights
    EXPECT_THROWS_AS(search.csr_search(std::vector<PointLonLatH>{{0., 0., 0.}}), exception::SearchError);
    EXPECT_THROWS_AS(search.search(PointLonLatH{0., 0., 0.}), exception::SearchError);

    // a height scale factor requires points with heights
    EXPECT_THROWS_AS(Search(grid, spec::Custom{{"height-scale-factor", 25.}}), exception::SearchError);
}


CASE("Search::csr_search: (lon, lat, height) queries, height-scale-factor") {
    std::mt19937 gen(7);
    std::uniform_real_distribution<double> height(0., 3000.);

    std::vector<double> lon;
    std::vector<double> lat;
    std::vector<double> h;
    for (const auto& p : random_lonlat(1000, 8)) {
        lon.push_back(p.lon());
        lat.push_back(p.lat());
        h.push_back(height(gen));
    }

    const grid::unstructured::UnstructuredLLH grid(lon, lat, h);
    const Search search(grid, spec::Custom{{"height-scale-factor", 25.}, {"search", "knn"}, {"search-k", 4}});

    const projection::LonLatHToXYZ llh(FigureFactory::build(grid.figure()), 25.);
    std::vector<PointLonLatH> queries;
    std::vector<PointXYZ> xyz;
    for (const auto& p : random_lonlat(100, 9)) {
        queries.emplace_back(p, height(gen));
        xyz.emplace_back(llh.fwd(queries.back()));
    }

    const auto a = search.csr_search(queries);
    const auto b = search.csr_search(xyz);
    EXPECT(a.offsets == b.offsets);
    EXPECT(a.indices == b.indices);
    EXPECT(a.distances == b.distances);

    // (lon, lat) queries would be at height 0
    EXPECT_THROWS_AS(search.csr_search(std::vector<PointLonLat>{{0., 0.}}), exception::SearchError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
