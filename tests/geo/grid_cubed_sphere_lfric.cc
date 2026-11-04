// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/grid/cubedsphere/LFRic.h"
#include "eckit/geo/projection/Stretch.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::cubedsphere::LFRic;


const Grid* make(const std::string& spec) {
    return GridFactory::make_from_string(spec);
}


double distance_degrees(const PointLonLat& a, const PointLonLat& b) {
    return figure::Sphere::centralAngle(a, b) * 180. / M_PI;
}


PointLonLat point(const Grid& grid, size_t k) {
    auto it = grid.begin();
    EXPECT(it += static_cast<Grid::Iterator::difference_type>(k));
    return std::get<PointLonLat>(*it);
}


CASE("names") {
    for (size_t n : {1, 2, 5}) {
        const auto N = std::to_string(n);

        for (const auto& [a, suffix, size] : std::vector<std::tuple<std::string, std::string, size_t>>{
                 {"C", "", 6 * n * n}, {"N", "-N", 6 * n * n + 2}, {"E", "-E", 12 * n * n}}) {
            std::unique_ptr<const Grid> grid(make("{grid: CS-LFR-" + N + suffix + "}"));
            EXPECT(grid->size() == size);
            EXPECT(grid->arrangement() == a);
            EXPECT(grid->type() == "cubed_sphere_lfric");
            EXPECT(grid->name() == "CS-LFR-" + N + suffix);
            EXPECT(grid->spec_str() == R"({"grid":"CS-LFR-)" + N + suffix + R"("})");

            // case-insensitive, by hyphen or underscore, and by type
            for (const auto& spec : {"{grid: cs-lfr-" + N + "-" + a + "}", "{grid: Cs_Lfr_" + N + "_" + a + "}",
                                     "{type: cubed_sphere_lfric, n: " + N + ", arrangement: " + a + "}"}) {
                std::unique_ptr<const Grid> same(make(spec));
                EXPECT(*same == *grid);
            }
        }

        std::unique_ptr<const Grid> c(make("{grid: cs_lfr_" + N + "}"));
        EXPECT(c->spec_str() == R"({"grid":"CS-LFR-)" + N + R"("})");
    }

    EXPECT_THROWS(make("{grid: CS-LFR-L-4}"));
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 0}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, arrangement: L}"), exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, equatorial_latitude: 90}"), exception::ProjectionError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, target_north_pole: [0, 10], target_null_island: [0, 10]}"),
                     exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, target_south_pole: [0, 10], target_north_pole: [0, 10]}"),
                     exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, target_south_pole: [0, 10], target_null_island: [0, 10]}"),
                     exception::GridError);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, target_north_pole: [0, 100]}"), BadValue);
    EXPECT_THROWS_AS(make("{type: cubed_sphere_lfric, n: 2, target_north_pole: [0]}"), exception::SpecError);
}


CASE("C48 (cubedsphere_mesh_generator)") {
    // reference points of a C48 LFRic mesh (UGRID, and edges as calc_edges): faces, nodes and edges, [lon, lat]
    struct Reference {
        size_t index;
        PointLonLat point;
    };

    for (const auto& [grid, references] : std::vector<std::pair<std::string, std::vector<Reference>>>{
             {"CS-LFR-48",
              {
                  {0, {315.93266075154253, 34.818741563821646}},      // panel 1, NW corner
                  {1, {317.80777901502904, 35.646161623112214}},      // ... eastwards
                  {2303, {44.067339248457408, -34.818741563821646}},  // panel 1, SE corner
                  {2304, {45.932660751542599, 34.818741563821646}},   // panel 2
                  {9216, {315., 36.148516240597395}},                 // panel 5
                  {11520, {45., -36.148516240597402}},                // panel 6
                  {13823, {225., -36.148516240597409}},               // last
              }},
             {"CS-LFR-48-N",
              {
                  {0, {315., 35.264389682754654}},       // panel 1, NW corner
                  {9216, {315., 37.051717987818385}},    // panel 5 (inner nodes)
                  {11425, {45., -35.264389682754654}},   // panel 6
                  {13825, {225., -35.264389682754654}},  // last
              }},
             {"CS-LFR-48-E",
              {
                  {0, {315.93244487939540, 35.69801098591473}},       // panel 1, cell 0, N
                  {1, {315., 34.39000657764789}},                     // ... W
                  {2, {315.93287206440851, 33.93946985834990}},       // ... S
                  {144, {315., 32.65966853483278}},                   // panel 1, row 1, cell 0, W
                  {18624, {315.94313270076753, 36.59171151180524}},   // panel 5, cell 0, S
                  {20880, {314.05686729923241, 36.59171151180524}},   // panel 5, cell 0, E
                  {27647, {225.94313270076756, -36.59171151180526}},  // last
              }},
         }) {
        std::unique_ptr<const Grid> g(make("{grid: " + grid + "}"));
        for (const auto& [k, ref] : references) {
            EXPECT(distance_degrees(point(*g, k), ref) < 1e-5);
        }
    }
}


CASE("by name, as cubed_sphere (the same points)") {
    struct test_t {
        double equatorial_latitude;
        PointLonLat target_north_pole;
    };

    for (const auto& [lat, pole] : std::vector<test_t>{{0., NORTH_POLE},
                                                       {30., NORTH_POLE},
                                                       {0., {-170., 70.}},
                                                       {20., {0., 50.}},
                                                       {-45., {120., -30.}},
                                                       {0., {30., 90.}}}) {
        for (const std::string a : {"C", "N", "E"}) {
            spec::Custom lfric{{"grid", "CS-LFR-4" + (a == "C" ? std::string{} : "-" + a)}};
            spec::Custom cubed_sphere{{"type", "cubed_sphere"}, {"n", 4}, {"arrangement", a}, {"panels", "lfric"}};

            if (lat != 0.) {
                lfric.set("equatorial_latitude", lat);
                cubed_sphere.set("stretching_factor", projection::Stretch::factor_from_equatorial_latitude(lat));
            }

            if (pole.lon() != NORTH_POLE.lon() || pole.lat() != NORTH_POLE.lat()) {
                // as rotate_mesh_coords, about the North Pole by (lon + 180), but for |lon| < 1e-4 [rad]
                lfric.set("target_north_pole", std::vector<double>{pole.lon(), pole.lat()});
                cubed_sphere.set("projection",
                                 new spec::Custom{{"type", "rotation"},
                                                  {"south_pole", std::vector<double>{pole.lon() + 180., -pole.lat()}},
                                                  {"rotation_angle", pole.lon() == 0. ? 180. : 0.}});
            }

            std::unique_ptr<const Grid> a_grid(GridFactory::build(lfric));
            std::unique_ptr<const Grid> b_grid(GridFactory::build(cubed_sphere));

            EXPECT(a_grid->type() == "cubed_sphere_lfric");
            EXPECT(b_grid->type() == "cubed_sphere");
            EXPECT(a_grid->size() == b_grid->size());
            EXPECT(a_grid->to_latlons() == b_grid->to_latlons());

            // recorded as the target South Pole
            std::unique_ptr<const Grid> c_grid(make(a_grid->spec_str()));
            EXPECT(*c_grid == *a_grid);
            EXPECT(c_grid->to_latlons() == a_grid->to_latlons());
        }
    }
}


CASE("stretching (equatorial_latitude) and rotation (target_south_pole, target_north_pole, target_null_island)") {
    const size_t n    = 4;
    const auto centre = (n / 2) * n + n / 2;                            // panel 1 centre node, (0, 0)
    const auto pole   = 4 * n * n + (n / 2 - 1) * (n - 1) + n / 2 - 1;  // panel 5 centre node, (0, 90)

    LFRic base(n, Arrangement::CUBED_SPHERE_N);
    EXPECT(distance_degrees(point(base, centre), {0., 0.}) < 1e-12);
    EXPECT(distance_degrees(point(base, pole), {0., 90.}) < 1e-12);

    std::unique_ptr<const Grid> stretched(make("{grid: CS-LFR-4-N, equatorial_latitude: 30}"));
    EXPECT(stretched->spec_str() == R"({"equatorial_latitude":30,"grid":"CS-LFR-4-N"})");
    EXPECT(distance_degrees(point(*stretched, centre), {0., 30.}) < 1e-12);
    EXPECT(distance_degrees(point(*stretched, pole), {0., 90.}) < 1e-12);

    std::unique_ptr<const Grid> rotated(make("{grid: CS-LFR-4-N, target_north_pole: [-170, 70]}"));
    EXPECT(rotated->spec_str() == R"({"grid":"CS-LFR-4-N","target_south_pole":[10,-70]})");
    EXPECT(distance_degrees(point(*rotated, pole), {-170., 70.}) < 1e-12);
    EXPECT(distance_degrees(point(*rotated, centre), {10., 20.}) < 1e-12);

    std::unique_ptr<const Grid> south_pole(make("{grid: CS-LFR-4-N, target_south_pole: [10, -70]}"));
    EXPECT(*south_pole == *rotated);
    EXPECT(south_pole->to_latlons() == rotated->to_latlons());

    std::unique_ptr<const Grid> null_island(make("{grid: CS-LFR-4-N, target_null_island: [10, 20]}"));
    EXPECT(*null_island == *rotated);

    // a target on the North Pole rotates about it by (lon + 180), as rotate_mesh_coords (not for lon = 0)
    std::unique_ptr<const Grid> about_pole(make("{grid: CS-LFR-4-N, target_north_pole: [30, 90]}"));
    EXPECT(about_pole->spec_str() == R"({"grid":"CS-LFR-4-N","target_south_pole":[210,-90]})");
    for (size_t k = 0; k < base.size(); ++k) {
        const auto p = point(base, k);
        EXPECT(distance_degrees(point(*about_pole, k), {p.lon() + 210., p.lat()}) < 1e-12);
    }

    std::unique_ptr<const Grid> not_rotated(make("{grid: CS-LFR-4-N, target_north_pole: [0, 90]}"));
    EXPECT(not_rotated->spec_str() == R"({"grid":"CS-LFR-4-N"})");
    EXPECT(not_rotated->to_latlons() == base.to_latlons());

    // a Null Island on the equator, the North Pole on the same longitude (as get_target_north_pole)
    std::unique_ptr<const Grid> equator(make("{grid: CS-LFR-4-N, target_null_island: [10, 0]}"));
    EXPECT(equator->spec_str() == R"({"grid":"CS-LFR-4-N","target_south_pole":[190,-90]})");

    std::unique_ptr<const Grid> both(
        make("{grid: CS-LFR-4-N, equatorial_latitude: 30, target_north_pole: [-170, 70]}"));
    std::unique_ptr<const Grid> rebuilt(make(both->spec_str()));
    EXPECT(*rebuilt == *both);
    EXPECT(rebuilt->to_latlons() == both->to_latlons());

    // cell centres and edge midpoints are the mean of the transformed nodes
    LFRic c(n, Arrangement::CUBED_SPHERE_C, 30., {10., -70.});
    LFRic v(n, Arrangement::CUBED_SPHERE_N, 30., {10., -70.});
    LFRic e(n, Arrangement::CUBED_SPHERE_E, 30., {10., -70.});
    EXPECT(distance_degrees(point(c, 0), point(v, 0)) < 90. / n);
    EXPECT(distance_degrees(point(e, 0), point(v, 0)) < 90. / n);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
