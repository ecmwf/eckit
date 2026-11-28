// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


// in the ballpark (independent of the platform): microdegrees, or micrometres
constexpr double TOLERANCE = 1e-6;


// British National Grid (OSGB36, Airy 1830 ellipsoid), less its false easting/northing (400000, -100000)
const std::string BNG = "{type: tmerc, lat_0: 49, lon_0: -2, k_0: 0.9996012717, a: 6377563.396, b: 6356256.909}";


const std::vector<std::string> SPECS{
    "{type: tmerc}",
    "{type: tmerc, lon_0: 9, k_0: 0.9996, figure: wgs84}",
    "{type: tmerc, lat_0: -35, lon_0: 147}",
    BNG,
};


/// Points within 30 degrees of the central meridian (as the Krüger series accuracy is stated within 3900 km)
std::vector<PointLonLat> points(const Projection& projection) {
    const auto lon_0 = projection.spec().get_double("lon_0", 0.);

    std::vector<PointLonLat> points;
    struct test_t {
        double dlon;  // from the central meridian
        double lat;
    };

    const std::vector<test_t> cases{{0., 0.}, {-3., 0.}, {1., 52.}, {-30., 35.}, {25., -70.}, {7., 89.9}, {-10., -90.}};
    for (const auto& test : cases) {
        points.emplace_back(PointLonLat::make(lon_0 + test.dlon, test.lat));
    }
    return points;
}


CASE("tmerc: spec, proj_str") {
    struct test_t {
        std::string spec;
        std::string spec_str;
        std::string proj_str;
    };

    const std::vector<test_t> tests{
        {SPECS[0], R"({"type":"tmerc"})", "+proj=tmerc +R=6371229"},
        {SPECS[1], R"({"figure":"wgs84","k_0":0.9996,"lon_0":9,"type":"tmerc"})",
         "+proj=tmerc +k_0=0.9996 +lon_0=9 +ellps=WGS84"},
        {"{type: transverse_mercator, lat_0: -35, lon_0: 147}", R"({"lat_0":-35,"lon_0":147,"type":"tmerc"})",
         "+proj=tmerc +lat_0=-35 +lon_0=147 +R=6371229"},
    };
    for (const auto& test : tests) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT(projection->spec_str() == test.spec_str);
        EXPECT(projection->proj_str() == test.proj_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("tmerc: fwd, inv") {
    for (const auto& spec : SPECS) {
        P tmerc(ProjectionFactory::make_from_string(spec));

        for (const auto& p : points(*tmerc)) {
            EXPECT(points_equal(tmerc->inv(tmerc->fwd(p)), p, TOLERANCE));
        }

        // the origin
        auto q = std::get<PointXY>(tmerc->fwd(std::get<PointLonLat>(tmerc->inv(PointXY{0., 0.}))));
        EXPECT(points_equal(q, PointXY{0., 0.}, TOLERANCE));
    }
}


CASE("tmerc: Ordnance Survey example") {
    // "A guide to coordinates systems in Great Britain" (Ordnance Survey, 2020), Annex C: Transverse Mercator
    // (E, N) = (651409.903, 313177.270) [m] of (lat, lon) = (52°39'27.2531"N, 1°43'4.5177"E) on OSGB36 (to the mm)
    P bng(ProjectionFactory::make_from_string(BNG));

    const auto p = PointLonLat{1. + 43. / 60. + 4.5177 / 3600., 52. + 39. / 60. + 27.2531 / 3600.};
    const auto q = PointXY{651409.903 - 400000., 313177.270 + 100000.};

    EXPECT(points_equal(std::get<PointXY>(bng->fwd(p)), q, 1e-3));
    EXPECT(points_equal(std::get<PointLonLat>(bng->inv(q)), p, 1e-8));
}


CASE("tmerc: as PROJ") {
    if (!ProjectionFactory::has_type("proj")) {
        return;
    }

    for (const auto& spec : SPECS) {
        P tmerc(ProjectionFactory::make_from_string(spec));
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", tmerc->proj_str()}}));

        for (const auto& p : points(*tmerc)) {
            EXPECT(points_equal(proj->inv(tmerc->fwd(p)), p, TOLERANCE));
            EXPECT(points_equal(tmerc->inv(proj->fwd(p)), p, TOLERANCE));
        }
    }
}


CASE("tmerc: lat_0 in [-90, 90], k_0 > 0") {
    const std::vector<std::string> specs{"{type: tmerc, lat_0: 91}", "{type: tmerc, lat_0: -100}",
                                         "{type: tmerc, k_0: 0}", "{type: tmerc, k_0: -1}"};
    for (const std::string& spec : specs) {
        EXPECT_THROWS_AS(P(ProjectionFactory::make_from_string(spec)), exception::ProjectionError);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
