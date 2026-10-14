// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/projection/PROJ.h"
#include "eckit/geo/projection/Reverse.h"
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


CASE("PROJ::proj_str") {
    using projection::PROJ;

    // key "proj" first, then the others (sorted), then the figure (default if not specified)
    EXPECT(PROJ::proj_str(spec::Custom{{"type", "mercator"}, {"lon_0", -180}}) == "+proj=merc +lon_0=-180 +R=6371229");
    EXPECT(PROJ::proj_str(spec::Custom{{"type", "mercator"}, {"lat_ts", 14}, {"figure", "wgs84"}}) ==
           "+proj=merc +lat_ts=14 +ellps=WGS84");
    EXPECT(PROJ::proj_str(spec::Custom{{"type", "lcc"}, {"lat_1", 30}, {"a", 6378206.4}, {"b", 6356583.8}}) ==
           "+proj=lcc +lat_0=0 +lat_1=30 +a=6378206.4 +b=6356583.8");
}


CASE("PROJ strings of grids") {
    struct test_t {
        std::string grid;
        std::string proj_str;
    };

    for (const auto& test : std::vector<test_t>{
             {"{grid: [1, 1]}", "+proj=longlat +R=6371229"},
             {"{grid: [1, 1], figure: wgs84}", "+proj=longlat +ellps=WGS84"},
             {"{grid: o32}", "+proj=longlat +R=6371229"},
             {"{grid: [1, 1], rotation: [-40, 10]}", "+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_proj=longlat +R=6371229"},
             {"{grid: o32, rotation: [-30, -20]}", "+proj=ob_tran +lon_0=-20 +o_lat_p=30 +o_proj=longlat +R=6371229"},
             {"{grid: [1, 1], projection: {type: rotation, south_pole: [10, -40], rotation_angle: 25}}",
              "+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_lon_p=25 +o_proj=longlat +R=6371229"},
             {"{x: [0, 2, 1], y: [0, 2, 1], projection: {type: proj, source: '+proj=lcc +lat_1=30 +R=6371229'}}",
              "+proj=lcc +lat_1=30 +R=6371229"},
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.grid));
        const auto& projection = grid->projection();
        EXPECT(projection.proj_str() == test.proj_str);

        // rotations as PROJ (ob_tran), to the rotated (lon, lat)
        if (projection.type() == "rotation") {
            P proj(ProjectionFactory::build(
                spec::Custom{{"type", "proj"}, {"source", "+proj=longlat +R=6371229"}, {"target", test.proj_str}}));

            for (const auto& p : std::vector<PointLonLat>{{0., 0.}, {10., 20.}, {-35., 50.}, {120., -30.}}) {
                EXPECT(points_equal(projection.fwd(p), proj->fwd(p), 1e-6));
            }
        }
    }
}


CASE("PROJ strings of projections") {
    struct test_t {
        std::string spec;
        std::string proj_str;
    };

    for (const auto& test : std::vector<test_t>{
             {"{type: none}", "+proj=longlat +R=6371229"},
             {"{type: eqc, lat_ts: 30, lat_0: 10}",
              "+proj=eqc +lat_0=10 +lat_ts=30 +to_meter=111198.923448546 +R=6371229"},
             {"{type: plate-carree, lat_ts: 30, figure: wgs84}",
              "+proj=eqc +lat_ts=30 +to_meter=111319.490793274 +R=6378137"},
             {"{type: aea, lat_0: 23, lat_1: 29.5, lat_2: 45.5, lon_0: -96}",
              "+proj=aea +lat_0=23 +lat_1=29.5 +lat_2=45.5 +lon_0=-96 +R=6371229"},
             {"{type: laea, lat_0: 52, lon_0: 10, figure: grs80}", "+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80"},
             {"{type: lambert_azimuthal_equal_area, lat_0: 90}", "+proj=laea +lat_0=90 +R=6371229"},
             {"{type: lcc, lat_1: 33, lat_2: 45, lon_0: -90, figure: wgs84}",
              "+proj=lcc +lat_0=0 +lat_1=33 +lat_2=45 +lon_0=-90 +ellps=WGS84"},
             {"{type: lambert_lam, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9, reverse: true}",
              "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229"},
             {"{type: lcc, lat_1: 40, lon_0: 357.5, figure: grib1}",
              "+proj=lcc +lat_0=0 +lat_1=40 +lon_0=357.5 +R=6367470"},
             {"{type: merc, lat_ts: 14}", "+proj=merc +lat_ts=14 +R=6371229"},
             {"{type: mercator, lon_0: -90, figure: wgs84}", "+proj=merc +lon_0=-90 +ellps=WGS84"},
             {"{type: stere, lat_0: 90, lat_ts: 60, lon_0: 280}",
              "+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229"},
             {"{type: polar_stereographic, lat_0: 90, lon_0: -45, figure: wgs84}",
              "+proj=stere +lat_0=90 +lon_0=-45 +ellps=WGS84"},
             {"{type: ll-to-xyz}", "+proj=cart +R=6371229"},
             {"{type: ll-to-xyz, figure: wgs84}", "+proj=cart +ellps=WGS84"},
             {"{type: rotation, south_pole: [10, -40]}",
              "+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_proj=longlat +R=6371229"},
             {"{type: proj, source: '+proj=lcc +lat_1=30 +R=6371229'}", "+proj=lcc +lat_1=30 +R=6371229"},
         }) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT(projection->proj_str() == test.proj_str);

        // the same coordinates as PROJ, from (lon, lat) (rotations are tested with grids)
        if (!LibEcKitGeo::projdb_is_available() || projection->type() == "rotation" || projection->type() == "proj" ||
            projection->reverse()) {
            continue;
        }

        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", test.proj_str}}));

        for (const auto& p : std::vector<PointLonLat>{{0., 30.}, {-12., 45.}, {-100., 60.}, {20., 80.}}) {
            EXPECT(points_equal(projection->fwd(p), proj->fwd(p), 1e-6));
        }
    }

    // no PROJ equivalent
    for (const auto& spec : std::vector<std::string>{
             "{type: space_view, h: 35785831}",
             "{type: stretch, stretching_factor: 2}",
         }) {
        P projection(ProjectionFactory::make_from_string(spec));
        EXPECT_THROWS_AS(projection->proj_str(), exception::ProjectionError);
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

        EXPECT(test.periodic == bbox->periodic());
        EXPECT(test.contains_north_pole == bbox->contains(NORTH_POLE));
        EXPECT(test.contains_south_pole == bbox->contains(SOUTH_POLE));

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

    constexpr double eps_ll = PointLonLat::EPS;  // [degree]
    constexpr double eps_xy = 1e-6;              // [m]
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


CASE("projection: proj, reverse from spec") {
    const std::string spec = "{type: proj, source: 'EPSG:4326', target: 'EPSG:3035'}";

    P plain(ProjectionFactory::make_from_string(spec));
    P reverse(ProjectionFactory::make_from_string(spec.substr(0, spec.size() - 1) + ", reverse: true}"));

    // the source is the default
    EXPECT(plain->spec_str() == R"({"target":"EPSG:3035","type":"proj"})");
    EXPECT(reverse->spec_str() == R"({"source":"EPSG:3035","type":"proj"})");
    EXPECT_NOT(plain->reverse());
    EXPECT_NOT(reverse->reverse());

    // identical, if built from the spec of another
    for (const auto* projection : {plain.get(), reverse.get()}) {
        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
        EXPECT(same->reverse() == projection->reverse());
    }

    // reversed: fwd as inv, and inv as fwd
    const PointLonLat p{10., 50.};
    const auto q = plain->fwd(p);
    EXPECT(points_equal(reverse->fwd(q), p));
    EXPECT(points_equal(reverse->inv(p), q, PointXY::EPS));

    // reversed (again, by projection::Reverse): the spec as "reverse": true, built as the swapped source/target
    struct reversed_t {
        const Projection& projection;
        const Projection& expected;
        std::string spec;
        Point a;  // expected source point
    };

    for (const auto& r : std::vector<reversed_t>{
             {*plain, *reverse, R"({"reverse":true,"target":"EPSG:3035","type":"proj"})", q},
             {*reverse, *plain, R"({"reverse":true,"source":"EPSG:3035","type":"proj"})", p},
         }) {
        const projection::Reverse reversed(ProjectionFactory::build(r.projection.spec()));
        EXPECT(reversed.spec_str() == r.spec);
        EXPECT(reversed.reverse());

        P same(ProjectionFactory::build(reversed.spec()));
        EXPECT(*same == r.expected);
        EXPECT_NOT(same->reverse());

        const auto b = r.expected.fwd(r.a);
        EXPECT(points_equal(reversed.fwd(r.a), b, PointXY::EPS));
        EXPECT(points_equal(reversed.inv(b), r.a, PointXY::EPS));

        // reversed twice, as not reversed
        const projection::Reverse twice(new projection::Reverse(ProjectionFactory::build(r.projection.spec())));
        EXPECT(twice.spec_str() == r.projection.spec_str());
        EXPECT(twice.reverse() == r.projection.reverse());
    }
}


CASE("projection: rotation, as PROJ") {
    struct test_t {
        std::string rotation;
        std::string spec;
        std::string spec_as_proj;
    };

    const std::vector<test_t> tests{
        {"{type: rotation, south_pole: [10, -40]}", R"({"south_pole":[10,-40],"type":"rotation"})",
         R"({"source":"+proj=longlat +R=6371229",)"
         R"("target":"+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_proj=longlat +R=6371229","type":"proj"})"},
        {"{type: rotation, south_pole: [10, -40], rotation_angle: 25}",
         R"({"rotation_angle":25,"south_pole":[10,-40],"type":"rotation"})",
         R"({"source":"+proj=longlat +R=6371229",)"
         R"("target":"+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_lon_p=25 +o_proj=longlat +R=6371229","type":"proj"})"},
    };

    for (const auto& test : tests) {
        P rotation(ProjectionFactory::make_from_string(test.rotation));
        EXPECT(rotation->spec_str() == test.spec);

        // from (lon, lat) to the rotated (lon, lat), on the figure
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"},
                                                     {"source", "+proj=longlat " + rotation->figure().proj_str()},
                                                     {"target", rotation->proj_str()}}));
        EXPECT(proj->spec_str() == test.spec_as_proj);

        // rebuilt from either spec, the same
        P from_spec(ProjectionFactory::build(rotation->spec()));
        P from_spec_as_proj(ProjectionFactory::build(proj->spec()));
        EXPECT(from_spec->spec_str() == test.spec);
        EXPECT(from_spec_as_proj->spec_str() == test.spec_as_proj);

        // the same coordinates, by all
        for (const auto& p : std::vector<PointLonLat>{{0., 0.}, {10., 20.}, {-35., 50.}, {120., -30.}}) {
            const auto q = rotation->fwd(p);
            const auto r = rotation->inv(p);

            for (const auto* projection : {proj.get(), from_spec.get(), from_spec_as_proj.get()}) {
                EXPECT(points_equal(projection->fwd(p), q, 1e-6));
                EXPECT(points_equal(projection->inv(p), r, 1e-6));
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
