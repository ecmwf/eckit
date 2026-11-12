// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <utility>
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


// as GRIB lambert_lam.grib2: LoV=4.9, LaD=Latin1=Latin2=51.967, on the default sphere (R=6371229)
const std::string LCC = "{type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9}";


CASE("lcc: spec") {
    struct test_t {
        std::string spec;
        std::string spec_str;
    };

    for (const auto& test : std::vector<test_t>{
             {"{type: lcc, lat_1: 33, lat_2: 45, lon_0: -90}", R"({"lat_1":33,"lat_2":45,"lon_0":-90,"type":"lcc"})"},
             {LCC, R"({"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"type":"lcc"})"},
             // lat_2 defaults to lat_1 (one standard parallel)
             {"{type: lcc, lat_0: 51.967, lat_1: 51.967, lat_2: 51.967, lon_0: 4.9}",
              R"({"lat_0":51.967,"lat_1":51.967,"lon_0":4.9,"type":"lcc"})"},
         }) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT(projection->spec_str() == test.spec_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("lcc: fwd, inv") {
    P lcc(ProjectionFactory::make_from_string(LCC));

    // the origin
    EXPECT(points_equal(lcc->fwd(PointLonLat{4.9, 51.967}), PointXY{0., 0.}, TOLERANCE));

    // the first grid point of lambert_lam.grib2
    const PointLonLat p{4.038959, 51.199433};
    const auto q = lcc->fwd(p);

    EXPECT(points_equal(q, PointXY{-60000.021039, -85000.055256}, TOLERANCE));
    EXPECT(points_equal(lcc->inv(q), p, TOLERANCE));
}


CASE("lcc: proj_str") {
    P lcc(ProjectionFactory::make_from_string(LCC));
    EXPECT(lcc->proj_str() == "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229");

    // the same projection, the other way
    P reverse(ProjectionFactory::make_from_string(LCC.substr(0, LCC.size() - 1) + ", reverse: true}"));
    EXPECT(reverse->proj_str() == lcc->proj_str());

    if (!ProjectionFactory::has_type("proj")) {
        return;
    }

    // as PROJ
    P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", lcc->proj_str()}}));

    for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {30., 70.}}) {
        EXPECT(points_equal(proj->inv(lcc->fwd(p)), p, TOLERANCE));
        EXPECT(points_equal(lcc->inv(proj->fwd(p)), p, TOLERANCE));
    }
}


CASE("lcc: on an ellipsoid, as PROJ") {
    for (const auto& spec :
         std::vector<std::string>{"{type: lcc, lat_1: 33, lat_2: 45, lon_0: -90, figure: wgs84}",
                                  "{type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9, figure: wgs84}"}) {
        P lcc(ProjectionFactory::make_from_string(spec));

        for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {-100., 70.}}) {
            EXPECT(points_equal(lcc->inv(lcc->fwd(p)), p, TOLERANCE));
        }

        if (!ProjectionFactory::has_type("proj")) {
            continue;
        }

        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", lcc->proj_str()}}));

        for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {-100., 70.}}) {
            EXPECT(points_equal(proj->inv(lcc->fwd(p)), p, TOLERANCE));
            EXPECT(points_equal(lcc->inv(proj->fwd(p)), p, TOLERANCE));
        }
    }
}


CASE("lcc: of the 2.5 km MEPS grids, on a sphere of R = 6371000, and with a southern cone") {
    struct test_t {
        std::string spec;
        std::string spec_str;
        std::string proj_str;
        std::vector<std::pair<PointLonLat, PointXY>> points;  // first and last grid points, as computed by PROJ
    };

    for (const auto& test : std::vector<test_t>{
             {"{type: lcc, lat_0: 63.3, lat_1: 63.3, lon_0: 15, figure: {R: 6371000}}",
              R"({"figure":{"r":6371000},"lat_0":63.3,"lat_1":63.3,"lon_0":15,"type":"lcc"})",
              "+proj=lcc +lat_0=63.3 +lat_1=63.3 +lon_0=15 +R=6371000",
              {
                  {{1.60491, 51.633816}, {-935084.015885738, -1207517.775780012}},               // meps-bris-nordic
                  {{49.418304333937, 71.246332962370}, {1184915.984114262, 1212482.224219988}},  //
                  {{0.278281, 50.319618}, {-1060083.933378283, -1332517.699323948}},             // meps-aila-2.0
                  {{54.241265999505, 71.576009822725}, {1309916.066621717, 1337482.300676052}},  //
              }},
             {"{type: lcc, lat_0: -41, lat_1: -41, lon_0: 172.5}",
              R"({"lat_0":-41,"lat_1":-41,"lon_0":172.5,"type":"lcc"})",
              "+proj=lcc +lat_0=-41 +lat_1=-41 +lon_0=172.5 +R=6371229",
              {
                  {{157.520469, -51.008413}, {-1059999.95891465, -1210000.045666926}},  // meps-bris-nz
                  {{-176.714272954551, -29.602117994275},
                   {1060000.04108535, 1209999.954333074}},  // across the date line
              }},
         }) {
        P lcc(ProjectionFactory::make_from_string(test.spec));

        EXPECT(lcc->spec_str() == test.spec_str);
        EXPECT(lcc->proj_str() == test.proj_str);

        for (const auto& [p, q] : test.points) {
            EXPECT(points_equal(lcc->fwd(p), q, TOLERANCE));
            EXPECT(points_equal(lcc->inv(q), p, TOLERANCE));
        }
    }
}


CASE("lcc: lat_1 and lat_2 in ]-90, 90[") {
    for (const std::string spec : {
             "{type: lcc, lat_0: 90, lat_1: 90, lat_2: 90, lon_0: 0}",
             "{type: lcc, lat_0: 45, lat_1: 45, lat_2: -90, lon_0: 0}",
             "{type: lcc, lat_0: -90, lat_1: -90, lat_2: -90, lon_0: 0}",
         }) {
        EXPECT_THROWS_AS(std::unique_ptr<const Projection>(ProjectionFactory::make_from_string(spec)),
                         exception::ProjectionError);
    }
}

}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
