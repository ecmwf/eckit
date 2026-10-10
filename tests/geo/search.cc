// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using Neighbours = Search::Neighbours;


std::vector<size_t> indices(const Neighbours& neighbours) {
    std::vector<size_t> result;
    for (const auto& n : neighbours) {
        result.push_back(n.index);
    }
    return result;
}


bool approx(double a, double b, double eps = 1e-12) {
    return std::abs(a - b) <= eps;
}


// distance-sorted (then index) neighbours, by brute force
Neighbours brute_force(const std::vector<PointXYZ>& points, const PointXYZ& q) {
    Neighbours all;
    for (size_t i = 0; i < points.size(); ++i) {
        all.push_back({points[i], i, PointXYZ::distance(points[i], q)});
    }

    std::sort(all.begin(), all.end(), [](const auto& a, const auto& b) {
        return a.distance < b.distance || (a.distance == b.distance && a.index < b.index);
    });
    return all;
}


Neighbours brute_force_knn(const std::vector<PointXYZ>& points, const PointXYZ& q, size_t k) {
    auto all = brute_force(points, q);
    all.resize(std::min(k, all.size()));
    return all;
}


Neighbours brute_force_radius(const std::vector<PointXYZ>& points, const PointXYZ& q, double radius) {
    auto all = brute_force(points, q);
    all.erase(std::remove_if(all.begin(), all.end(), [radius](const auto& n) { return n.distance > radius; }),
              all.end());
    return all;
}


std::vector<PointXYZ> random_points_on_unit_sphere(size_t n, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> lon(-180., 180.);
    std::uniform_real_distribution<double> sinlat(-1., 1.);

    projection::LonLatToXYZ to_xyz(1.);

    std::vector<PointXYZ> points;
    points.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        points.emplace_back(to_xyz.fwd(PointLonLat{lon(gen), std::asin(sinlat(gen)) * 180. / M_PI}));
    }
    return points;
}


// points along the X axis: 0, 1, 2, 3 and 10
const std::vector<PointXYZ> LINE{{0., 0., 0.}, {1., 0., 0.}, {2., 0., 0.}, {3., 0., 0.}, {10., 0., 0.}};


const std::vector<std::string> TREES{"memory", "mapped-anonymous-memory", "mapped-temporary-file", "mapped-cache-file"};


CASE("Search: examples") {
    const Search search(LINE, spec::Custom{{{"search-tree", "memory"}}});
    EXPECT_EQUAL(search.size(), LINE.size());

    SECTION("search_nn: nearest point") {
        auto a = search.search_nn(PointXYZ{0.4, 0., 0.});
        EXPECT_EQUAL(a.index, 0);
        EXPECT(a.point == LINE[0]);
        EXPECT(approx(a.distance, 0.4));

        auto b = search.search_nn(PointXYZ{9., 1., 0.});
        EXPECT_EQUAL(b.index, 4);
        EXPECT(approx(b.distance, std::sqrt(2.)));
    }

    SECTION("search_knn: k nearest points, by increasing distance") {
        EXPECT(indices(search.search_knn(PointXYZ{1.4, 0., 0.}, 2)) == std::vector<size_t>({1, 2}));
        EXPECT(indices(search.search_knn(PointXYZ{1.4, 0., 0.}, 3)) == std::vector<size_t>({1, 2, 0}));
        EXPECT(indices(search.search_knn(PointXYZ{1.4, 0., 0.}, 1)) == std::vector<size_t>({1}));

        // k = 0: nothing; k > size: everything
        EXPECT(search.search_knn(PointXYZ{1.4, 0., 0.}, 0).empty());
        EXPECT(indices(search.search_knn(PointXYZ{1.4, 0., 0.}, 100)) == std::vector<size_t>({1, 2, 0, 3, 4}));
    }

    SECTION("search_radius: points within radius (inclusive), by increasing distance") {
        EXPECT(indices(search.search_radius(PointXYZ{0., 0., 0.}, 1.5)) == std::vector<size_t>({0, 1}));
        EXPECT(indices(search.search_radius(PointXYZ{0., 0., 0.}, 1.)) == std::vector<size_t>({0, 1}));
        EXPECT(indices(search.search_radius(PointXYZ{2.9, 0., 0.}, 1.)) == std::vector<size_t>({3, 2}));
        EXPECT(indices(search.search_radius(PointXYZ{2., 0., 0.}, 0.)) == std::vector<size_t>({2}));
        EXPECT(search.search_radius(PointXYZ{6., 0., 0.}, 1.).empty());

        EXPECT_THROWS_AS(search.search_radius(PointXYZ{0., 0., 0.}, -1.), exception::SearchError);
    }

    SECTION("search_knn_or_radius: union") {
        // radius dominates
        EXPECT(indices(search.search_knn_or_radius(PointXYZ{0., 0., 0.}, 1, 2.5)) == std::vector<size_t>({0, 1, 2}));

        // k dominates
        EXPECT(indices(search.search_knn_or_radius(PointXYZ{0., 0., 0.}, 4, 0.5)) == std::vector<size_t>({0, 1, 2, 3}));

        // nothing within radius, still k nearest
        EXPECT(indices(search.search_knn_or_radius(PointXYZ{6., 0., 0.}, 1, 1.)) == std::vector<size_t>({3}));
    }

    SECTION("search_knn_and_radius: intersection") {
        // radius limits
        EXPECT(indices(search.search_knn_and_radius(PointXYZ{0., 0., 0.}, 3, 1.5)) == std::vector<size_t>({0, 1}));

        // k limits
        EXPECT(indices(search.search_knn_and_radius(PointXYZ{0., 0., 0.}, 2, 100.)) == std::vector<size_t>({0, 1}));

        // nothing within radius
        EXPECT(search.search_knn_and_radius(PointXYZ{6., 0., 0.}, 2, 1.).empty());
    }
}


CASE("Search: search(point), configured behaviour") {
    const PointXYZ q{1.4, 0., 0.};

    auto make = [](const spec::Custom::container_type& c) {
        auto custom = spec::Custom{c};
        custom.set("search-tree", std::string{"memory"});
        return std::make_unique<Search>(LINE, custom);
    };

    SECTION("nn (default)") {
        auto search = make({});
        EXPECT(indices(search->search(q)) == std::vector<size_t>({1}));

        EXPECT(indices(make({{"search", "nn"}})->search(q)) == std::vector<size_t>({1}));
    }

    SECTION("knn") {
        auto search = make({{"search", "knn"}, {"search-k", 3}});
        EXPECT(search->search(q) == search->search_knn(q, 3));
        EXPECT(indices(search->search(q)) == std::vector<size_t>({1, 2, 0}));
    }

    SECTION("radius") {
        auto search = make({{"search", "radius"}, {"search-radius", 1.5}});
        EXPECT(search->search(q) == search->search_radius(q, 1.5));
        EXPECT(indices(search->search(q)) == std::vector<size_t>({1, 2, 0}));
    }

    SECTION("knn_or_radius") {
        auto search = make({{"search", "knn_or_radius"}, {"search-k", 4}, {"search-radius", 0.5}});
        EXPECT(search->search(q) == search->search_knn_or_radius(q, 4, 0.5));
        EXPECT(indices(search->search(q)) == std::vector<size_t>({1, 2, 0, 3}));
    }

    SECTION("knn_and_radius") {
        auto search = make({{"search", "knn_and_radius"}, {"search-k", 4}, {"search-radius", 0.5}});
        EXPECT(search->search(q) == search->search_knn_and_radius(q, 4, 0.5));
        EXPECT(indices(search->search(q)) == std::vector<size_t>({1}));
    }

    SECTION("invalid configuration") {
        EXPECT_THROWS_AS(make({{"search", "unknown"}}), exception::SearchError);
        EXPECT_THROWS_AS(make({{"search", "knn"}}), exception::SearchError);
        EXPECT_THROWS_AS(make({{"search", "radius"}}), exception::SearchError);
        EXPECT_THROWS_AS(make({{"search", "radius"}, {"search-radius", -1.}}), exception::SearchError);
        EXPECT_THROWS_AS(make({{"search", "knn_or_radius"}, {"search-k", 1}}), exception::SearchError);
        EXPECT_THROWS_AS(make({{"search", "knn_and_radius"}, {"search-radius", 1.}}), exception::SearchError);
    }

    SECTION("invalid tree") {
        EXPECT_THROWS_AS(Search(LINE, spec::Custom{{{"search-tree", "unknown"}}}), exception::SearchError);
        EXPECT_THROWS_AS(Search(std::vector<PointXYZ>{}, spec::Custom{{{"search-tree", "memory"}}}),
                         exception::SearchError);
    }
}


CASE("Search: (lon, lat) points") {
    // unit sphere, as configured
    const spec::Custom spec{{{"search-tree", "memory"}, {"R", 1.}}};

    projection::LonLatToXYZ to_xyz(1.);
    const std::vector<PointXYZ> points{to_xyz.fwd(PointLonLat{0., 0.}), to_xyz.fwd(PointLonLat{90., 0.}),
                                       to_xyz.fwd(PointLonLat{180., 0.}), to_xyz.fwd(PointLonLat{0., 90.})};

    const Search search(points, spec);

    EXPECT_EQUAL(search.search_nn(PointLonLat{10., 10.}).index, 0);
    EXPECT_EQUAL(search.search_nn(PointLonLat{100., 10.}).index, 1);
    EXPECT_EQUAL(search.search_nn(PointLonLat{-170., 10.}).index, 2);
    EXPECT_EQUAL(search.search_nn(PointLonLat{0., 80.}).index, 3);

    // (lon, lat) and (X, Y, Z) queries are equivalent
    EXPECT(indices(search.search_knn(PointLonLat{45., 45.}, 4)) ==
           indices(search.search_knn(to_xyz.fwd(PointLonLat{45., 45.}), 4)));

    // chord length between the pole and the equator, on the unit sphere (equator points are equidistant)
    auto r = search.search_radius(PointLonLat{0., 90.}, std::sqrt(2.) + 1e-9);
    EXPECT_EQUAL(r.size(), 4);
    EXPECT_EQUAL(r.front().index, 3);
    for (size_t i = 1; i < r.size(); ++i) {
        EXPECT(approx(r[i].distance, std::sqrt(2.), 1e-9));
    }

    EXPECT_THROWS_AS(search.search_nn(PointXY{0., 0.}), exception::SearchError);
}


CASE("Search: grid") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));
    ASSERT(grid);

    const Search search(*grid, spec::Custom{{{"search-tree", "memory"}}});
    EXPECT_EQUAL(search.size(), grid->size());

    const auto nx = 360 / 30;

    size_t index = 0;
    for (const auto& p : *grid) {
        const auto& q = std::get<PointLonLat>(p);

        auto nn = search.search_nn(q);
        EXPECT(approx(nn.distance, 0., 1e-6));

        // poles are represented by multiple (coincident) points
        if (q.pole()) {
            EXPECT_EQUAL(search.search_radius(q, 1.).size(), nx);
        }
        else {
            EXPECT_EQUAL(nn.index, index);
            EXPECT_EQUAL(search.search_radius(q, 1.).size(), 1);
        }

        ++index;
    }
}


CASE("Search: grid, on the grid figure") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));
    const auto R = grid->figure().R();  // spherical Earth

    const Search search(*grid, spec::Custom{{{"search-tree", "memory"}}});

    // equidistant to two points on the equator (lon = 0 and 30): chords subtending 15 degrees
    auto knn = search.search_knn(PointLonLat{15., 0.}, 2);
    EXPECT_EQUAL(knn.size(), 2);

    for (const auto& n : knn) {
        EXPECT(approx(n.distance, 2. * R * std::sin(7.5 * M_PI / 180.), 1e-6));
    }
}


CASE("Search: grid points find themselves, on their figure") {
    std::vector<std::string> grids{R"({"grid": [30, 30]})", R"({"grid": [30, 30], "figure": "wgs84"})"};

    if (ProjectionFactory::has_type("proj") && LibEcKitGeo::projdb_is_available()) {
        // (lon, lat) on the PROJ source CRS, and geocentric points
        grids.emplace_back("swisslv95");
        grids.emplace_back(
            R"({"type": "regular_xy", "grid": [1000000, 1000000], "bounding_box_xy": [-2000000, -2000000, 2000000, 2000000],)"
            R"( "projection": {"type": "proj", "source": "+proj=geocent +ellps=WGS84", "target": "EPSG:3857"}})");
    }

    for (const auto& g : grids) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(g));
        const Search search(*grid, spec::Custom{{{"search-tree", "memory"}}});

        for (const auto& p : *grid) {
            EXPECT(search.search_nn(p).distance < 1e-6);
        }
    }
}


CASE("Grid::search") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));

    // the same search, for the grid lifetime
    const auto& search = grid->search();
    EXPECT(&search == &grid->search());
    EXPECT_EQUAL(search.size(), grid->size());

    const Search reference(*grid, spec::Custom{{{"search-tree", "memory"}}});

    // away from ties (e.g. the poles, represented by coincident points)
    for (const auto& p : {PointLonLat{14., 1.}, PointLonLat{-100., 44.}, PointLonLat{170., -50.}}) {
        auto a = search.search_knn(p, 4);
        auto b = reference.search_knn(p, 4);

        EXPECT(indices(a) == indices(b));
        for (size_t i = 0; i < a.size(); ++i) {
            EXPECT(approx(a[i].distance, b[i].distance, 1e-6));
        }
    }
}


CASE("Search: all trees, compared to brute force") {
    const auto points  = random_points_on_unit_sphere(1000, 42);
    const auto queries = random_points_on_unit_sphere(50, 7);

    constexpr size_t k   = 7;
    constexpr double rad = 0.2;

    for (const auto& tree : TREES) {
        for (bool fast : {true, false}) {
            SECTION(tree + (fast ? " (fast build)" : " (build by insertion)")) {
                const Search search(points, spec::Custom{{{"search-tree", tree}, {"search-fast-build", fast}}});
                EXPECT_EQUAL(search.size(), points.size());

                for (const auto& q : queries) {
                    const auto all = brute_force(points, q);

                    auto nn = search.search_nn(q);
                    EXPECT_EQUAL(nn.index, all.front().index);
                    EXPECT(approx(nn.distance, all.front().distance));
                    EXPECT(nn.point == points[nn.index]);

                    EXPECT(search.search_knn(q, k) == brute_force_knn(points, q, k));
                    EXPECT(search.search_radius(q, rad) == brute_force_radius(points, q, rad));

                    // union/intersection of prefixes (by distance) of the same sorted list: the longest/shortest
                    auto knn    = brute_force_knn(points, q, k);
                    auto radius = brute_force_radius(points, q, rad);
                    EXPECT(search.search_knn_or_radius(q, k, rad) == (knn.size() > radius.size() ? knn : radius));
                    EXPECT(search.search_knn_and_radius(q, k, rad) == (knn.size() < radius.size() ? knn : radius));
                }
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
