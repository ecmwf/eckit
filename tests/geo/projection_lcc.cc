// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


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
        EXPECT_EQUAL(projection->spec_str(), test.spec_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("lcc: fwd, inv") {
    P lcc(ProjectionFactory::make_from_string(LCC));

    // the origin
    EXPECT(points_equal(lcc->fwd(PointLonLat{4.9, 51.967}), PointXY{0., 0.}));

    // the first grid point of lambert_lam.grib2
    const PointLonLat p{4.038959, 51.199433};
    const auto q = lcc->fwd(p);

    EXPECT(points_equal(q, PointXY{-60000.02103865735, -85000.05525572484}));
    EXPECT(points_equal(lcc->inv(q), p));
}


CASE("lcc: proj_str") {
    P lcc(ProjectionFactory::make_from_string(LCC));
    EXPECT_EQUAL(lcc->proj_str(), "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229");

    // the same projection, the other way
    P reverse(ProjectionFactory::make_from_string(LCC.substr(0, LCC.size() - 1) + ", reverse: true}"));
    EXPECT_EQUAL(reverse->proj_str(), lcc->proj_str());

    if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
        return;
    }

    // as PROJ
    P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", lcc->proj_str()}}));

    for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {30., 70.}}) {
        EXPECT(points_equal(proj->inv(lcc->fwd(p)), p));
        EXPECT(points_equal(lcc->inv(proj->fwd(p)), p));
    }
}


CASE("lcc: on an ellipsoid, as PROJ") {
    for (const auto& spec :
         std::vector<std::string>{"{type: lcc, lat_1: 33, lat_2: 45, lon_0: -90, figure: wgs84}",
                                  "{type: lcc, lat_0: 51.967, lat_1: 51.967, lon_0: 4.9, figure: wgs84}"}) {
        P lcc(ProjectionFactory::make_from_string(spec));

        for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {-100., 70.}}) {
            EXPECT(points_equal(lcc->inv(lcc->fwd(p)), p));
        }

        if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
            continue;
        }

        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", lcc->proj_str()}}));

        for (const auto& p : std::vector<PointLonLat>{{4.038959, 51.199433}, {-20., 30.}, {-100., 70.}}) {
            EXPECT(points_equal(proj->inv(lcc->fwd(p)), p));
            EXPECT(points_equal(lcc->inv(proj->fwd(p)), p));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
