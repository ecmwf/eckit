// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/geo/cache/CropCache.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


using A = std::unique_ptr<const Area>;
using G = std::unique_ptr<const Grid>;


// two disjoint rings, off the 1-degree grid points: 10 x 10 and 10 x 9 points inside
const std::string RING_1 = "{latitudes: [0.5, 0.5, 10.5, 10.5], longitudes: [0.5, 10.5, 10.5, 0.5]}";
const std::string RING_2 = "{latitudes: [-19.5, -19.5, -10.5, -10.5], longitudes: [100.5, 110.5, 110.5, 100.5]}";


std::string polygons_spec() {
    G ring_1(GridFactory::make_from_string(RING_1));
    G ring_2(GridFactory::make_from_string(RING_2));
    return "{type: polygon, uid: ['" + ring_1->uid() + "', '" + ring_2->uid() + "']}";
}

const std::vector<std::string> BBOXES{
    "{type: bounding_box, north: 10, west: -10, south: -10, east: 10}",  // across Greenwich
    "{type: bounding_box, north: 60.5, west: 349.5, south: 40.5, east: 30.5}",
};


/// Points (lon, lat) contained in a bounding box (to PointLonLat::EPS), by index (independently of Area::contains)
Grid::renumber_type contained(const Grid& grid, double n, double w, double s, double e) {
    constexpr auto eps    = PointLonLat::EPS;
    const auto [lat, lon] = grid.to_latlons();

    Grid::renumber_type indices;
    for (size_t i = 0; i < lat.size(); ++i) {
        if (s - eps <= lat[i] && lat[i] <= n + eps &&
            PointLonLat::normalise_angle_to_minimum(lon[i], w - eps) <= e + eps) {
            indices.push_back(i);
        }
    }
    return indices;
}


/// The cropped grid points (lon, lat) are those of the grid, by index
bool same_points(const Grid& cropped, const Grid& grid, const Grid::renumber_type& indices) {
    const auto [lat, lon]   = grid.to_latlons();
    const auto [clat, clon] = cropped.to_latlons();

    if (clat.size() != indices.size()) {
        return false;
    }

    for (size_t i = 0; i < indices.size(); ++i) {
        if (!points_equal(PointLonLat{clon[i], clat[i]}, PointLonLat{lon[indices[i]], lat[indices[i]]})) {
            return false;
        }
    }
    return true;
}


std::set<size_t> as_set(const Grid::renumber_type& indices) {
    return {indices.begin(), indices.end()};
}


CASE("polygon area: spec") {
    G ring_1(GridFactory::make_from_string(RING_1));
    G ring_2(GridFactory::make_from_string(RING_2));
    EXPECT(ring_1->type() == "unstructured_ll");

    A polygons(AreaFactory::make_from_string(polygons_spec()));
    EXPECT(polygons->type() == "polygon");
    EXPECT(polygons->spec_str() ==
           R"({"type":"polygon","uid":[")" + ring_1->uid() + R"(",")" + ring_2->uid() + R"("]})");

    A same(AreaFactory::make_from_string(polygons->spec_str()));
    EXPECT(*same == *polygons);

    EXPECT(polygons->contains(PointLonLat{5., 5.}));
    EXPECT(polygons->contains(PointLonLat{105., -15.}));
    EXPECT_NOT(polygons->contains(PointLonLat{50., 0.}));

    // one ring, by its coordinates
    A ring(AreaFactory::make_from_string(
        "{type: polygon, latitudes: [0.5, 0.5, 10.5, 10.5], longitudes: [0.5, 10.5, 10.5, 0.5]}"));
    EXPECT(ring->spec_str() == R"({"type":"polygon","uid":")" + ring_1->uid() + R"("})");

    // as built from points
    const area::Polygon from_points(area::Polygon::container_type{area::Polygon::value_type{
        PointLonLat{0.5, 0.5}, PointLonLat{10.5, 0.5}, PointLonLat{10.5, 10.5}, PointLonLat{0.5, 10.5}}});
    EXPECT(from_points == *ring);

    // rings of three points or more
    for (const auto& spec : {
             "{type: polygon, latitudes: [0, 0, 10], longitudes: [0, 10]}",
             "{type: polygon, latitudes: [0, 0], longitudes: [0, 10]}",
             "{type: polygon, latitudes: [0, 0, 10]}",
             "{type: polygon}",
         }) {
        EXPECT_THROWS_AS(A(AreaFactory::make_from_string(spec)), exception::AreaError);
    }
}


CASE("crop: unprojected grids") {
    for (const auto& spec : {"{grid: 1/1}", "{grid: 1/1, reference: [0.5, 0.5]}", "{grid: O32}"}) {
        G grid(GridFactory::make_from_string(spec));
        const auto points = grid->to_points();

        SECTION(std::string(spec) + ", bounding boxes: as the cropped grid") {
            for (const auto& bbox_spec : BBOXES) {
                A bbox(AreaFactory::make_from_string(bbox_spec));
                const auto& b = dynamic_cast<const area::BoundingBox&>(*bbox);

                const auto indices = grid->crop(*bbox);
                EXPECT(!indices.empty());
                EXPECT(as_set(indices) == as_set(contained(*grid, b.north(), b.west(), b.south(), b.east())));

                // the same points, in the same order
                G cropped(grid->make_grid_cropped(*bbox));
                const auto cropped_points = cropped->to_points();

                EXPECT(cropped_points.size() == indices.size());
                for (size_t i = 0; i < indices.size(); ++i) {
                    EXPECT(points_equal(cropped_points[i], points[indices[i]]));
                }
            }
        }

        SECTION(std::string(spec) + ", polygons: as their bounding boxes") {
            A polygons(AreaFactory::make_from_string(polygons_spec()));

            auto expected = as_set(contained(*grid, 10.5, 0.5, 0.5, 10.5));
            expected.merge(as_set(contained(*grid, -10.5, 100.5, -19.5, 110.5)));

            const auto indices = grid->crop(*polygons);
            EXPECT(as_set(indices) == expected);
            EXPECT(std::is_sorted(indices.begin(), indices.end()));

            if (grid->type() == "regular_ll" && points.size() == 360 * 181) {
                EXPECT(indices.size() == 10 * 10 + 10 * 9);
            }
        }
    }
}


CASE("crop: global grids, by a global bounding box from another longitude") {
    // all points, rows from longitude -180 (180), the grids from longitude 0
    A bbox(new area::BoundingBox(90., -180., -90., 360.));

    for (const auto& [spec, type] : std::vector<std::pair<std::string, std::string>>{
             {"{grid: 9/9}", "regular_ll"},
             {"{grid: O2}", "reduced_gg"},
         }) {
        G grid(GridFactory::make_from_string(spec));
        const auto indices = grid->crop(*bbox);

        EXPECT(indices.size() == grid->size());
        EXPECT(as_set(indices).size() == grid->size());
        EXPECT_NOT(std::is_sorted(indices.begin(), indices.end()));

        // each row from longitude 180 (as -180)
        const auto [lat, lon] = grid->to_latlons();
        EXPECT(
            types::is_approximately_equal(PointLonLat::normalise_angle_to_minimum(lon[indices.front()], -180.), -180.));
        for (size_t i = 1; i < indices.size(); ++i) {
            if (lat[indices[i]] != lat[indices[i - 1]]) {
                EXPECT(types::is_approximately_equal(PointLonLat::normalise_angle_to_minimum(lon[indices[i]], -180.),
                                                     -180.));
            }
        }

        // the cropped grid, of the same type, has the same points in the same order
        G cropped(grid->make_grid_cropped(*bbox));
        EXPECT(cropped->type() == type);
        EXPECT(same_points(*cropped, *grid, indices));
    }
}


CASE("crop: scanning modes (as the grid), cropping out odd and even numbers of starting rows or columns") {
    // the cropped grid keeps the scanning mode (alternating rows, or columns, start in the same direction)
    const std::vector<std::string> orders{"i+j-",  "i+j+",  "i-j-",  "i-j+",  "j+i-",  "j-i+",  "i+-j-",
                                          "i-+j+", "i+-j+", "i-+j-", "j+-i+", "j-+i-", "j-+i+", "j+-i-"};

    struct test_t {
        std::string grid;
        std::vector<double> area;  // [north, west, south, east]
    };

    for (const auto& test : std::vector<test_t>{
             // 10 columns, 11 rows: cropping out 1 (odd) or 2 (even) rows and columns at each side
             {"{grid: [1, 1], area: [10, 0, 0, 9]", {9, 1, 1, 8}},
             {"{grid: [1, 1], area: [10, 0, 0, 9]", {8, 2, 2, 7}},
             {"{grid: [1, 1], area: [10, 0, 0, 9]", {9, 2, 2, 8}},
             // Gaussian: cropping out 1 row and column at each side
             {"{grid: F4", {65., 22.5, -65., 315.}},
             // global, by a bounding box from another longitude (rows from -180, as 180)
             {"{grid: [30, 30]", {60., -180., -60., 150.}},
         }) {
        for (const auto& order : orders) {
            G grid(GridFactory::make_from_string(test.grid + ", order: " + order + "}"));
            A bbox(new area::BoundingBox(test.area[0], test.area[1], test.area[2], test.area[3]));

            G cropped(grid->make_grid_cropped(*bbox));
            EXPECT(cropped->type() == grid->type());
            EXPECT(cropped->order() == order);

            const auto indices = grid->crop(*bbox);
            EXPECT(as_set(indices).size() == cropped->size());
            EXPECT(same_points(*cropped, *grid, indices));
        }
    }
}


CASE("crop: alternating rows (or columns), cropping out 1, 2 or 3 starting rows (or columns), the cropped points") {
    // the cropped grid points, and the grid points by the indices
    struct test_t {
        std::string grid;
        std::vector<double> area;  // [north, west, south, east]
        std::vector<PointLonLat> expected;
    };

    // rows (i then j): 3 columns (longitude 0 to 2), 4 rows (latitude 3 to 0)
    const std::string rows_ipm = "{grid: [1, 1], area: [3, 0, 0, 2], order: i+-j-}";
    const std::string rows_imp = "{grid: [1, 1], area: [3, 0, 0, 2], order: i-+j-}";

    // columns (j then i): 4 columns (longitude 0 to 3), 3 rows (latitude 0 to 2)
    const std::string cols_jpm = "{grid: [1, 1], area: [2, 0, 0, 3], order: j+-i+}";
    const std::string cols_jmp = "{grid: [1, 1], area: [2, 0, 0, 3], order: j-+i+}";

    for (const auto& test : std::vector<test_t>{
             {rows_ipm, {2, 0, 0, 2}, {{0, 2}, {1, 2}, {2, 2}, {2, 1}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {2, 0}}},
             {rows_ipm, {1, 0, 0, 2}, {{0, 1}, {1, 1}, {2, 1}, {2, 0}, {1, 0}, {0, 0}}},
             {rows_ipm, {0, 0, 0, 2}, {{0, 0}, {1, 0}, {2, 0}}},
             {rows_imp, {2, 0, 0, 2}, {{2, 2}, {1, 2}, {0, 2}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {1, 0}, {0, 0}}},
             {rows_imp, {1, 0, 0, 2}, {{2, 1}, {1, 1}, {0, 1}, {0, 0}, {1, 0}, {2, 0}}},
             {rows_imp, {0, 0, 0, 2}, {{2, 0}, {1, 0}, {0, 0}}},
             {cols_jpm, {2, 1, 0, 3}, {{1, 0}, {1, 1}, {1, 2}, {2, 2}, {2, 1}, {2, 0}, {3, 0}, {3, 1}, {3, 2}}},
             {cols_jpm, {2, 2, 0, 3}, {{2, 0}, {2, 1}, {2, 2}, {3, 2}, {3, 1}, {3, 0}}},
             {cols_jpm, {2, 3, 0, 3}, {{3, 0}, {3, 1}, {3, 2}}},
             {cols_jmp, {2, 1, 0, 3}, {{1, 2}, {1, 1}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {3, 2}, {3, 1}, {3, 0}}},
             {cols_jmp, {2, 2, 0, 3}, {{2, 2}, {2, 1}, {2, 0}, {3, 0}, {3, 1}, {3, 2}}},
             {cols_jmp, {2, 3, 0, 3}, {{3, 2}, {3, 1}, {3, 0}}},
         }) {
        G grid(GridFactory::make_from_string(test.grid));
        A bbox(new area::BoundingBox(test.area[0], test.area[1], test.area[2], test.area[3]));

        G cropped(grid->make_grid_cropped(*bbox));
        EXPECT(cropped->order() == grid->order());

        const auto cropped_points = cropped->to_points();
        EXPECT(cropped_points.size() == test.expected.size());
        for (size_t k = 0; k < test.expected.size(); ++k) {
            EXPECT(points_equal(cropped_points[k], test.expected[k]));
        }

        const auto indices = grid->crop(*bbox);
        const auto points  = grid->to_points();
        EXPECT(indices.size() == test.expected.size());
        for (size_t k = 0; k < test.expected.size(); ++k) {
            EXPECT(points_equal(points[indices[k]], test.expected[k]));
        }
    }
}


CASE("crop: projected grids (rotation, lcc)") {
    struct test_t {
        std::string grid;
        double n;
        double w;
        double s;
        double e;
    };

    for (const auto& test : std::vector<test_t>{
             {"{grid: [1, 1], rotation: [-40, 10]}", 10., -10., -10., 10.},
             {"{x: [0, 120000, 2500], y: [0, 170000, 2500], false: [60000.021, 85000.055], "
              "projection: {type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9, reverse: true}}",
              52., 4.5, 51.5, 5.5},
         }) {
        G grid(GridFactory::make_from_string(test.grid));
        A bbox(new area::BoundingBox(test.n, test.w, test.s, test.e));

        // points in the area (geographic), as the grid
        const auto indices = grid->crop(*bbox);
        EXPECT(!indices.empty());
        EXPECT(indices == contained(*grid, test.n, test.w, test.s, test.e));

        const auto n = std::to_string(test.n);
        const auto w = std::to_string(test.w);
        const auto s = std::to_string(test.s);
        const auto e = std::to_string(test.e);

        A polygon(AreaFactory::make_from_string("{type: polygon, latitudes: [" + s + ", " + s + ", " + n + ", " + n +
                                                "], longitudes: [" + w + ", " + e + ", " + e + ", " + w + "]}"));
        EXPECT(as_set(grid->crop(*polygon)).size() <= indices.size());

        // projected grids crop as unstructured grids, of the same points
        G cropped(grid->make_grid_cropped(*bbox));
        EXPECT(cropped->type() == "unstructured_ll");
        EXPECT(same_points(*cropped, *grid, indices));
    }
}


CASE("crop: as the grid type, or unstructured (exchangeable by uid)") {
    struct test_t {
        std::string grid;
        std::string area;
        std::string type;
    };

    const std::string bbox = BBOXES.front();
    const std::string lcc =
        "{x: [0, 120000, 2500], y: [0, 170000, 2500], false: [60000.021, 85000.055], "
        "projection: {type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9, reverse: true}}";

    for (const auto& test : std::vector<test_t>{
             {"{grid: 1/1}", bbox, "regular_ll"},
             {"{grid: F32}", bbox, "regular_gg"},
             {"{grid: O32}", bbox, "reduced_gg"},
             {"{grid: 1/1}", polygons_spec(), "unstructured_ll"},
             {"{grid: [1, 1], rotation: [-40, 10]}", bbox, "unstructured_ll"},
             {lcc, "{type: bounding_box, north: 52, west: 4.5, south: 51.5, east: 5.5}", "unstructured_ll"},
         }) {
        G grid(GridFactory::make_from_string(test.grid));
        A area(AreaFactory::make_from_string(test.area));
        const auto indices = grid->crop(*area);

        G cropped(grid->make_grid_cropped(*area));
        EXPECT(cropped->type() == test.type);
        EXPECT(same_points(*cropped, *grid, indices));

        // as unstructured, always: its coordinates cached, exchangeable by uid
        G unstructured(grid->make_grid_cropped_unstructured_ll(*area));
        EXPECT(unstructured->type() == "unstructured_ll");
        EXPECT(same_points(*unstructured, *grid, indices));

        G by_uid(GridFactory::make_from_string("{uid: " + unstructured->uid() + "}"));
        EXPECT(same_points(*by_uid, *grid, indices));
    }

    // nothing to crop (between the grid points)
    G grid(GridFactory::make_from_string("{grid: 1/1}"));
    EXPECT_THROWS_AS(G(grid->make_grid_cropped_unstructured_ll(area::BoundingBox(0.6, 0.4, 0.4, 0.6))),
                     exception::GridError);
}


CASE("crop: cache") {
    auto& cache = cache::CropCache::instance();
    cache.clear();

    // (regular grids are cropped directly, other grids by the points in the area, cached)
    G grid(GridFactory::make_from_string("{grid: O32}"));
    A bbox(AreaFactory::make_from_string(BBOXES.front()));

    const auto statistics = cache.statistics();
    const auto a          = grid->crop(*bbox);
    const auto b          = grid->crop(*bbox);

    EXPECT(a == b);
    EXPECT(cache.statistics().misses == statistics.misses + 1);
    EXPECT(cache.statistics().hits == statistics.hits + 1);
    EXPECT(cache.size() == 1);
    EXPECT(cache.usage().memory() > 0);

    // on disk, loaded once the in-memory cache is cleared
    if (LibEcKitGeo::caching()) {
        const auto dir = PathName{LibEcKitGeo::cacheDir()} / "crop" / std::to_string(cache::version::CROP);
        EXPECT(dir.exists());

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        dir.children(files, dirs);
        EXPECT(std::any_of(files.begin(), files.end(), [](const auto& f) { return f.extension() == ".crop"; }));

        cache.clear();
        EXPECT(grid->crop(*bbox) == a);
    }

    cache.clear();
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
