// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


CASE("projdb") {
    // eckit uses its own PROJ context. Reconfiguring it (projdb_reset()) recreates the context and re-reads PROJ_DATA
    // if set, so these transitions are deterministic regardless of whether the database was already accessed.


    SECTION("available by default") {
        LibEcKitGeo::projdb_reset();

        EXPECT(LibEcKitGeo::projdb_is_available());
    }


    SECTION("unavailable when pointed at a bad database") {
        LibEcKitGeo::projdb_set_search_paths("/does/not/exist/proj.db", {"/does/not/exist"});

        EXPECT(!LibEcKitGeo::projdb_is_available());

        LibEcKitGeo::projdb_reset();

        EXPECT(LibEcKitGeo::projdb_is_available());
    }


    SECTION("unavailable when PROJ_DATA points nowhere") {
        ::setenv("PROJ_DATA", "/does/not/exist", 1);
        LibEcKitGeo::projdb_reset();

        EXPECT(!LibEcKitGeo::projdb_is_available());

        ::unsetenv("PROJ_DATA");
        LibEcKitGeo::projdb_reset();

        EXPECT(LibEcKitGeo::projdb_is_available());
    }
}


CASE("projection: proj") {
    const std::string LL = "EPSG:4326";

    constexpr double eps = 1e-6;

    PointLonLat a{12., 55.};

    struct {
        const Point b;
        const std::string source;
        const std::string target;
    } tests_proj[] = {
        {PointXY{691875.632137542, 6098907.825129169}, LL, "+proj=utm +zone=32 +datum=WGS84"},
        {PointXY{691875.632137542, 6098907.825129169}, LL, "EPSG:32632"},
        {a, LL, LL},
        {a, LL, "EPSG:4979"},
        {PointXYZ{3586469.6567764, 762327.65877826, 5201383.5232023}, LL, "EPSG:4978"},
        {PointXYZ{3574529.7050235, 759789.74368715, 5219005.2599833}, LL, "+proj=cart +R=6371229."},
        {PointXYZ{3574399.5431832, 759762.07693392, 5218815.216709}, LL, "+proj=cart +ellps=sphere"},
        {a, LL, "+proj=latlon +ellps=sphere"},
    };

    for (const auto& test : tests_proj) {
        P projection(ProjectionFactory::build(
            spec::Custom{{{"type", "proj"}, {"source", test.source}, {"target", test.target}}}));

#if 0
Log::info() << "ellipsoid: '" << PROJ::ellipsoid(projection.target())
          << std::endl;
#endif

        auto b = projection->fwd(a);
        auto c = projection->inv(b);

        EXPECT(points_equal(b, test.b, eps));
        EXPECT(points_equal(c, a, eps));

        P reverse(ProjectionFactory::build(
            spec::Custom({{"type", "proj"}, {"source", test.target}, {"target", test.source}})));

        auto d = reverse->fwd(test.b);
        auto e = reverse->inv(d);

        EXPECT(points_equal(d, a, eps));
        EXPECT(points_equal(e, test.b, eps));
    }

    P polar_stereographic_north(ProjectionFactory::build(spec::Custom{
        {{"type", "proj"}, {"source", LL}, {"target", "+proj=stere +lat_0=90. +lon_0=-30. +R=6371229."}}}));

    P polar_stereographic_south(ProjectionFactory::build(spec::Custom{
        {{"type", "proj"}, {"source", LL}, {"target", "+proj=stere +lat_0=-90. +lon_0=-30. +R=6371229."}}}));

    struct {
        const P& projection;
        const PointXY min;
        const PointXY max;
        const bool periodic;
        const bool contains_north_pole;
        const bool contains_south_pole;
    } tests_bbox[] = {
        {polar_stereographic_north, {-2e6, -2e6}, {2e6, 2e6}, true, true, false},
        {polar_stereographic_north, {-2e6, -2e6}, {1e6, 1e6}, true, true, false},
        {polar_stereographic_north, {-2e6, -2e6}, {-1e6, -1e6}, false, false, false},
        {polar_stereographic_north, {-1e6, -1e6}, {2e6, 2e6}, true, true, false},
        {polar_stereographic_north, {-1e6, -1e6}, {1e6, 1e6}, true, true, false},
        {polar_stereographic_north, {1e6, 1e6}, {2e6, 2e6}, false, false, false},
        {polar_stereographic_south, {-2e6, -2e6}, {2e6, 2e6}, true, false, true},
        {polar_stereographic_south, {-2e6, -2e6}, {1e6, 1e6}, true, false, true},
        {polar_stereographic_south, {-2e6, -2e6}, {-1e6, -1e6}, false, false, false},
        {polar_stereographic_south, {-1e6, -1e6}, {2e6, 2e6}, true, false, true},
        {polar_stereographic_south, {-1e6, -1e6}, {1e6, 1e6}, true, false, true},
        {polar_stereographic_south, {1e6, 1e6}, {2e6, 2e6}, false, false, false},
    };

    for (const auto& test : tests_bbox) {
        auto bbox = area::BoundingBox::make_from_projection(test.min, test.max, *test.projection);
        ASSERT(bbox);

        EXPECT_EQUAL(test.periodic, bbox->periodic());
        EXPECT_EQUAL(test.contains_north_pole, bbox->contains(NORTH_POLE));
        EXPECT_EQUAL(test.contains_south_pole, bbox->contains(SOUTH_POLE));

        auto global = test.periodic && test.contains_north_pole && test.contains_south_pole;

        EXPECT(global == bbox->global());
    }
}


CASE("EPSG:4326 / EPSG:2056") {
    const std::string LL = "EPSG:4326";
    const std::string XY = "EPSG:2056";

    constexpr double eps_ll = 1e-6;  // [degree]
    constexpr double eps_xy = 1e-3;  // [m]

    struct {
        const PointLonLat ll;
        const PointXY xy;
    } tests[] = {
        {{8.5403, 47.3779}, {2683196.612242693, 1248035.313847182}},  // Zurich
        {{7.4441, 46.9465}, {2600416.254716625, 1199490.540613972}},  // Bern
        {{6.1557, 46.2074}, {2500986.154176505, 1118138.645411988}},  // Geneva
    };

    P to_xy(ProjectionFactory::build(spec::Custom{{{"type", "proj"}, {"source", LL}, {"target", XY}}}));
    P to_ll(ProjectionFactory::build(spec::Custom{{{"type", "proj"}, {"source", XY}, {"target", LL}}}));

    for (const auto& test : tests) {
        EXPECT(points_equal(to_xy->fwd(test.ll), test.xy, eps_xy));
        EXPECT(points_equal(to_xy->inv(test.xy), test.ll, eps_ll));

        EXPECT(points_equal(to_ll->fwd(test.xy), test.ll, eps_ll));
        EXPECT(points_equal(to_ll->inv(test.ll), test.xy, eps_xy));
    }
}


CASE("projection: proj vectors") {
    using v = std::vector<double>;

    auto approx = [](const v& a, const v& b, double eps) {
        return a.size() == b.size() &&
               std::equal(a.begin(), a.end(), b.begin(), [eps](double x, double y) { return std::abs(x - y) < eps; });
    };

    constexpr double eps_ll = 1e-9;  // [degree]
    constexpr double eps_xy = 1e-6;  // [m]
    constexpr double A      = 6378137.;


    SECTION("lonlat to xy (EPSG:4326 to EPSG:3857)") {
        P merc(ProjectionFactory::build(spec::Custom{{{"type", "proj"}, {"target", "EPSG:3857"}}}));

        const v lon{0., 10.};
        const v lat{0., 20.};
        const auto xy = merc->fwd(lon, lat);

        EXPECT(approx(xy[0], v{0., 1113194.9079327357}, eps_xy));
        EXPECT(approx(xy[1], v{0., 2273030.9269876895}, eps_xy));

        const auto ll = merc->inv(xy[0], xy[1]);
        EXPECT(approx(ll[0], lon, eps_ll));
        EXPECT(approx(ll[1], lat, eps_ll));

        // same as the single point
        const auto q = std::get<PointXY>(merc->fwd(PointLonLat{10., 20.}));
        EXPECT(approx(v{q.X(), q.Y()}, v{xy[0][1], xy[1][1]}, eps_xy));
    }


    SECTION("lonlat to xyz (EPSG:4326 to EPSG:4978), and back") {
        P ecef(ProjectionFactory::build(spec::Custom{{{"type", "proj"}, {"target", "EPSG:4978"}}}));

        const v lon{0., 90.};
        const v lat{0., 0.};
        const auto xyz = ecef->fwd(lon, lat);  // 2 to 3 coordinates

        EXPECT(approx(xyz[0], v{A, 0.}, eps_xy));
        EXPECT(approx(xyz[1], v{0., A}, eps_xy));
        EXPECT(approx(xyz[2], v{0., 0.}, eps_xy));

        const auto ll = ecef->inv(xyz[0], xyz[1], xyz[2]);  // 3 to 2 coordinates
        EXPECT(approx(ll[0], lon, eps_ll));
        EXPECT(approx(ll[1], lat, eps_ll));
    }


    SECTION("failure is NaN, for the failing points only") {
        P merc(ProjectionFactory::build(spec::Custom{{{"type", "proj"}, {"target", "EPSG:3857"}}}));

        const auto xy = merc->fwd(v{0., 0., 10.}, v{0., 91., 20.});  // invalid latitude

        EXPECT(std::isnan(xy[0][1]) && std::isnan(xy[1][1]));
        EXPECT(approx(v{xy[0][0], xy[0][2]}, v{0., 1113194.9079327357}, eps_xy));
    }


    SECTION("longitudes in [lon_minimum, lon_minimum + 360)") {
        P ll(ProjectionFactory::build(
            spec::Custom{{{"type", "proj"}, {"target", "EPSG:4326"}, {"lon_minimum", -180.}}}));

        const auto lonlat = ll->fwd(v{190., -10.}, v{0., 0.});
        EXPECT(approx(lonlat[0], v{-170., -10.}, eps_ll));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
