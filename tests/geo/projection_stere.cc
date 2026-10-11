// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


// in the ballpark (independent of the platform): microdegrees, or micrometres
constexpr double TOLERANCE = 1e-6;


const std::vector<std::string> SPECS{
    "{type: stere, lat_0: 90, lat_ts: 60, lon_0: 280}",
    "{type: stere, lat_0: -90}",
    "{type: stere, lat_0: 90, lon_0: -45, figure: wgs84}",
    "{type: stere, lat_0: -90, lat_ts: -71, lon_0: 0, figure: wgs84}",
};


bool is_north(const std::string& spec) {
    return spec.find("lat_0: 90") != std::string::npos;
}


// on the hemisphere of the pole
std::vector<PointLonLat> points(const std::string& spec) {
    const auto sign = is_north(spec) ? 1. : -1.;
    return {{-75., sign * 35.}, {268.656832, sign * 38.7017}, {10., sign * 60.}};
}


CASE("stere: spec, proj_str") {
    struct test_t {
        std::string spec;
        std::string spec_str;
        std::string proj_str;
    };

    for (const auto& test : std::vector<test_t>{
             {SPECS[0], R"({"lat_0":90,"lat_ts":60,"lon_0":280,"type":"stere"})",
              "+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229"},
             // lat_ts defaults to lat_0
             {SPECS[1], R"({"lat_0":-90,"type":"stere"})", "+proj=stere +lat_0=-90 +R=6371229"},
             {"{type: polar_stereographic, lat_0: -90, lat_ts: -90}", R"({"lat_0":-90,"type":"stere"})",
              "+proj=stere +lat_0=-90 +R=6371229"},
             {SPECS[2], R"({"figure":"wgs84","lat_0":90,"lon_0":-45,"type":"stere"})",
              "+proj=stere +lat_0=90 +lon_0=-45 +ellps=WGS84"},
         }) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT_EQUAL(projection->spec_str(), test.spec_str);
        EXPECT_EQUAL(projection->proj_str(), test.proj_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("stere: polar only") {
    EXPECT_THROWS_AS(P(ProjectionFactory::make_from_string("{type: stere, lat_0: 45}")), exception::ProjectionError);
}


CASE("stere: fwd, inv") {
    for (const auto& spec : SPECS) {
        P stere(ProjectionFactory::make_from_string(spec));

        // the pole is the origin
        const auto& pole = is_north(spec) ? NORTH_POLE : SOUTH_POLE;
        EXPECT(points_equal(stere->fwd(pole), PointXY{0., 0.}, TOLERANCE));

        for (const auto& p : points(spec)) {
            EXPECT(points_equal(stere->inv(stere->fwd(p)), p, TOLERANCE));
        }
    }
}


CASE("stere: as PROJ") {
    if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
        return;
    }

    for (const auto& spec : SPECS) {
        P stere(ProjectionFactory::make_from_string(spec));
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", stere->proj_str()}}));

        for (const auto& p : points(spec)) {
            EXPECT(points_equal(proj->inv(stere->fwd(p)), p, TOLERANCE));
            EXPECT(points_equal(stere->inv(proj->fwd(p)), p, TOLERANCE));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
