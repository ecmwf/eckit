// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <limits>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


const std::vector<std::string> SPECS{
    "{type: merc, lat_ts: 14}",
    "{type: merc, lon_0: -180}",
    "{type: merc, lat_ts: 14, figure: wgs84}",
};


CASE("merc: spec, proj_str") {
    struct test_t {
        std::string spec;
        std::string spec_str;
        std::string proj_str;
    };

    for (const auto& test : std::vector<test_t>{
             {SPECS[0], R"({"lat_ts":14,"type":"merc"})", "+proj=merc +lat_ts=14 +R=6371229"},
             {SPECS[1], R"({"lon_0":-180,"type":"merc"})", "+proj=merc +lon_0=-180 +R=6371229"},
             {SPECS[2], R"({"figure":"wgs84","lat_ts":14,"type":"merc"})", "+proj=merc +lat_ts=14 +ellps=WGS84"},
             {"{type: mercator, lat_ts: 14}", R"({"lat_ts":14,"type":"merc"})", "+proj=merc +lat_ts=14 +R=6371229"},
         }) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT_EQUAL(projection->spec_str(), test.spec_str);
        EXPECT_EQUAL(projection->proj_str(), test.proj_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("merc: fwd, inv") {
    for (const auto& spec : SPECS) {
        P merc(ProjectionFactory::make_from_string(spec));

        const PointLonLat p{-75., 35.};
        EXPECT(points_equal(merc->inv(merc->fwd(p)), p));

        EXPECT(std::get<PointXY>(merc->fwd(NORTH_POLE)).Y() > std::numeric_limits<double>::max());
        EXPECT(std::get<PointXY>(merc->fwd(SOUTH_POLE)).Y() < std::numeric_limits<double>::lowest());
    }
}


CASE("merc: as PROJ") {
    if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
        return;
    }

    for (const auto& spec : SPECS) {
        P merc(ProjectionFactory::make_from_string(spec));
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", merc->proj_str()}}));

        for (const auto& p : std::vector<PointLonLat>{{-75., 35.}, {262.036499, 14.736453}, {10., -60.}}) {
            EXPECT(points_equal(proj->inv(merc->fwd(p)), p));
            EXPECT(points_equal(merc->inv(proj->fwd(p)), p));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
