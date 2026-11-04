// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/grid/CubedSphere.h"
#include "eckit/geo/grid/cubedsphere/LFRic.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::CubedSphere;


const Grid* make(const std::string& spec) {
    return GridFactory::make_from_string(spec);
}


double distance_degrees(const PointLonLat& a, const PointLonLat& b) {
    return figure::Sphere::centralAngle(a, b) * 180. / M_PI;
}


/// Panels written out (spec), as named
std::string written_out(const CubedSphere::Panels& panels) {
    auto list = [](const auto& values) {
        std::string s;
        for (const auto& v : values) {
            s += (s.empty() ? "" : ", ") + v;
        }
        return "[" + s + "]";
    };

    auto block = [](const CubedSphere::Block& b) {
        return std::to_string(b.panel) + ", " + std::to_string(b.i0) + ", " + std::to_string(b.i1) + ", " +
               std::to_string(b.j0) + ", " + std::to_string(b.j1);
    };

    std::vector<std::string> frames;
    std::vector<std::string> nodes;
    std::vector<std::string> edges;
    std::vector<std::string> sides;

    for (const auto& f : panels.frames) {
        for (const auto& x : f) {
            frames.push_back(std::to_string(static_cast<long>(x)));
        }
    }

    for (const auto& b : panels.nodes) {
        nodes.push_back(block(b));
    }

    for (const auto& e : panels.edges) {
        edges.push_back(block(e.cells));
        sides.push_back(e.sides);
    }

    return "{frames: " + list(frames) + ", nodes: " + list(nodes) + ", edges: " + list(edges) +
           ", sides: " + list(sides) + "}";
}


CASE("spec") {
    for (size_t n : {1, 2, 5}) {
        const auto N = std::to_string(n);

        std::unique_ptr<const Grid> c(make("{type: cubed_sphere, n: " + N + ", panels: lfric}"));
        EXPECT(c->size() == 6 * n * n);
        EXPECT(c->shape() == (std::vector<size_t>{6, n, n}));
        EXPECT(c->arrangement() == "C");
        EXPECT(c->type() == "cubed_sphere");
        EXPECT(c->name().empty());
        EXPECT(c->spec_str() == R"({"n":)" + N + R"(,"panels":"lfric","type":"cubed_sphere"})");

        std::unique_ptr<const Grid> v(make("{type: cubed_sphere, n: " + N + ", arrangement: N, panels: lfric}"));
        EXPECT(v->size() == 6 * n * n + 2);
        EXPECT(v->shape() == (std::vector<size_t>{6 * n * n + 2}));
        EXPECT(v->spec_str() == R"({"arrangement":"N","n":)" + N + R"(,"panels":"lfric","type":"cubed_sphere"})");

        std::unique_ptr<const Grid> e(make("{type: cubed_sphere, n: " + N + ", arrangement: E, panels: lfric}"));
        EXPECT(e->size() == 12 * n * n);
        EXPECT(e->shape() == (std::vector<size_t>{12 * n * n}));
        EXPECT(e->spec_str() == R"({"arrangement":"E","n":)" + N + R"(,"panels":"lfric","type":"cubed_sphere"})");

        for (const auto* grid : {c.get(), v.get(), e.get()}) {
            std::unique_ptr<const Grid> same(make(grid->spec_str()));
            EXPECT(*same == *grid);
        }
    }
}


CASE("panels, written out") {
    for (size_t n : {1, 2, 3}) {
        const auto N      = std::to_string(n);
        const auto panels = written_out(grid::cubedsphere::LFRic::panels(n));

        for (const std::string a : {"C", "N", "E"}) {
            std::unique_ptr<const Grid> named(
                make("{type: cubed_sphere, n: " + N + ", arrangement: " + a + ", panels: lfric}"));
            std::unique_ptr<const Grid> written(
                make("{type: cubed_sphere, n: " + N + ", arrangement: " + a + ", panels: " + panels + "}"));

            EXPECT(written->to_latlons() == named->to_latlons());

            // the spec writes the panels out
            std::unique_ptr<const Grid> same(make(written->spec_str()));
            EXPECT(*same == *written);
            EXPECT(same->to_latlons() == written->to_latlons());
        }
    }
}


CASE("nodes and edges are unique, cell centres are within cells") {
    auto unique = [](const Grid& grid) {
        auto [lat, lon] = grid.to_latlons();

        std::set<std::pair<long long, long long>> points;
        for (size_t k = 0; k < grid.size(); ++k) {
            auto p = PointLonLat::make(lon[k], lat[k]);
            points.emplace(std::llround(p.lon() * 1e8) % 36000000000LL, std::llround(p.lat() * 1e8));
        }
        return points.size();
    };

    for (size_t n : {1, 2, 3, 8}) {
        const auto N = std::to_string(n);

        std::unique_ptr<const Grid> c(make("{type: cubed_sphere, n: " + N + ", panels: lfric}"));
        std::unique_ptr<const Grid> v(make("{type: cubed_sphere, n: " + N + ", arrangement: N, panels: lfric}"));
        std::unique_ptr<const Grid> e(make("{type: cubed_sphere, n: " + N + ", arrangement: E, panels: lfric}"));

        EXPECT(unique(*c) == 6 * n * n);
        EXPECT(unique(*v) == 6 * n * n + 2);
        EXPECT(unique(*e) == 12 * n * n);

        // a cell centre (or an edge midpoint) is closer to some node than the panel diagonal over n
        const auto d = 109.47122063449069 / static_cast<double>(n);
        for (const auto* grid : {c.get(), e.get()}) {
            for (const auto& p : *grid) {
                const auto q = std::get<PointLonLat>(p);
                EXPECT(std::any_of(v->begin(), v->end(), [&q, d](const Point& r) {
                    return distance_degrees(std::get<PointLonLat>(r), q) < d;
                }));
            }
        }
    }
}


CASE("iteration, first and last points") {
    for (const std::string a : {"C", "N", "E"}) {
        std::unique_ptr<const Grid> grid(make("{type: cubed_sphere, n: 3, arrangement: " + a + ", panels: lfric}"));
        auto [lat, lon] = grid->to_latlons();

        size_t k = 0;
        for (const auto& p : *grid) {
            const auto q = std::get<PointLonLat>(p);
            EXPECT(q.lat() == lat[k] && q.lon() == lon[k]);
            ++k;
        }
        EXPECT(k == grid->size());

        EXPECT(points_equal(grid->first_point(), PointLonLat{lon.front(), lat.front()}));
        EXPECT(points_equal(grid->last_point(), PointLonLat{lon.back(), lat.back()}));
    }
}


CASE("stretching and rotation") {
    const std::string base = "type: cubed_sphere, n: 4, arrangement: N, panels: lfric";
    const size_t centre    = 2 * 4 + 2;  // panel 1 centre node, (0, 0)

    auto point = [](const Grid& grid, size_t k) {
        auto it = grid.begin();
        EXPECT(it += static_cast<Grid::Iterator::difference_type>(k));
        return std::get<PointLonLat>(*it);
    };

    // Schmidt transform, the equator to latitude 30 for c = sqrt((1 - sin 30) / (1 + sin 30)) = 1 / sqrt(3)
    std::unique_ptr<const Grid> stretched(make("{" + base + ", stretching_factor: 0.5773502691896258}"));
    EXPECT(distance_degrees(point(*stretched, centre), {0., 30.}) < 1e-12);

    // rotation, from the rotated sphere (by projection, or the 'rotation' shorthand [lat, lon] of the South Pole)
    std::unique_ptr<const Grid> rotated(make("{" + base + ", projection: {type: rotation, south_pole: [10, -70]}}"));
    std::unique_ptr<const Grid> shorthand(make("{" + base + ", rotation: [-70, 10]}"));
    EXPECT(*rotated == *shorthand);
    EXPECT(distance_degrees(point(*rotated, centre), {10., 20.}) < 1e-12);

    // the spec prints 15 significant digits, so a stretching factor (not as such) round-trips approximately
    for (const auto* grid : {stretched.get(), rotated.get()}) {
        std::unique_ptr<const Grid> same(make(grid->spec_str()));
        EXPECT(*same == *grid);
        for (size_t k = 0; k < grid->size(); ++k) {
            EXPECT(distance_degrees(point(*same, k), point(*grid, k)) < 1e-12);
        }
    }

    EXPECT(stretched->spec_str() ==
           R"({"arrangement":"N","n":4,"panels":"lfric","stretching_factor":0.577350269189626,"type":"cubed_sphere"})");
    EXPECT(rotated->spec_str() ==
           R"({"arrangement":"N","n":4,"panels":"lfric","projection":{"south_pole":[10,-70],"type":"rotation"},)"
           R"("type":"cubed_sphere"})");
}


CASE("errors") {
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 0, panels: lfric}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2, panels: unknown}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2, panels: lfric, arrangement: L}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2, panels: lfric, projection: {type: eqc}}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2, panels: lfric, stretching_factor: 0}"),
                     exception::ProjectionError);

    // written out: frames, nodes and edges numbering (as arrangements need)
    const auto panels = written_out(grid::cubedsphere::LFRic::panels(2));
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 2, panels: {frames: [1, 0, 0]}}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 3, arrangement: N, panels: " + panels + "}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere, n: 3, arrangement: E, panels: " + panels + "}"), exception::GridError);
}


CASE("first and last points") {
    for (const std::string spec : {"{type: cubed_sphere, n: 4, panels: lfric}", "{grid: CS-LFR-4}",
                                   "{grid: CS-LFR-4-N}", "{grid: CS-LFR-4-E}"}) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));

        EXPECT(points_equal(grid->first_point(), grid->point(0)));
        EXPECT(points_equal(grid->first_point(), *grid->begin()));
        EXPECT(points_equal(grid->last_point(), grid->point(grid->size() - 1)));
        EXPECT(points_equal(grid->last_point(), grid->to_points().back()));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
