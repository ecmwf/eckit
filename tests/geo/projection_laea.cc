// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<const Projection>;


// in the ballpark (independent of the platform): microdegrees, or micrometres
constexpr double TOLERANCE = 1e-6;


struct test_t {
    std::string spec;
    PointLonLat origin;
    std::vector<PointLonLat> points;  // on the hemisphere of the origin
};


const std::vector<test_t> TESTS{
    {"{type: laea, lat_0: 52, lon_0: 10, figure: grs80}", {10., 52.}, {{-35.034024, 66.982143}, {41.1397, 23.9423}}},
    {"{type: laea, lat_0: 52, lon_0: 10}", {10., 52.}, {{-35.034024, 66.982143}, {41.1397, 23.9423}}},
    {"{type: laea}", {0., 0.}, {{-35., 66.}, {41., -23.}}},
    {"{type: laea, lat_0: 90, lon_0: -45}", {0., 90.}, {{-35., 66.}, {141., 23.}}},
    {"{type: laea, lat_0: -90, figure: wgs84}", {0., -90.}, {{-35., -66.}, {141., -23.}}},
};


CASE("laea: spec, proj_str") {
    struct spec_t {
        std::string spec;
        std::string spec_str;
        std::string proj_str;
    };

    for (const auto& test : std::vector<spec_t>{
             {TESTS[0].spec, R"({"figure":"grs80","lat_0":52,"lon_0":10,"type":"laea"})",
              "+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80"},
             {TESTS[2].spec, R"({"type":"laea"})", "+proj=laea +R=6371229"},
             {"{type: lambert_azimuthal_equal_area, lat_0: -90, figure: wgs84}",
              R"({"figure":"wgs84","lat_0":-90,"type":"laea"})", "+proj=laea +lat_0=-90 +ellps=WGS84"},
         }) {
        P projection(ProjectionFactory::make_from_string(test.spec));
        EXPECT(projection->spec_str() == test.spec_str);
        EXPECT(projection->proj_str() == test.proj_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);
    }
}


CASE("laea: fwd, inv") {
    for (const auto& test : TESTS) {
        P laea(ProjectionFactory::make_from_string(test.spec));

        EXPECT(points_equal(laea->fwd(test.origin), PointXY{0., 0.}, TOLERANCE));

        for (const auto& p : test.points) {
            EXPECT(points_equal(laea->inv(laea->fwd(p)), p, TOLERANCE));
        }
    }
}


CASE("laea: as PROJ") {
    if (!ProjectionFactory::has_type("proj")) {
        return;
    }

    for (const auto& test : TESTS) {
        P laea(ProjectionFactory::make_from_string(test.spec));
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", laea->proj_str()}}));

        for (const auto& p : test.points) {
            EXPECT(points_equal(proj->inv(laea->fwd(p)), p, TOLERANCE));
            EXPECT(points_equal(laea->inv(proj->fwd(p)), p, TOLERANCE));
        }
    }
}


CASE("laea: near the poles, on an ellipsoid") {
    // latitudes (of the authalic latitude) of points near the pole [m]; as PROJ, of an accuracy limited by the formula
    // near the poles (about 0.1 %, at 1 m)
    for (const std::string spec :
         {"{type: laea, lat_0: 90, figure: wgs84}", "{type: laea, lat_0: -90, figure: wgs84}"}) {
        P laea(ProjectionFactory::make_from_string(spec));
        const auto north = spec.find("lat_0: 90") != std::string::npos;

        EXPECT(points_equal(laea->inv(PointXY{0., 0.}), north ? NORTH_POLE : SOUTH_POLE, TOLERANCE));

        auto previous = north ? NORTH_POLE.lat() : SOUTH_POLE.lat();
        for (const double d : {1., 10., 100., 1000., 1e4}) {
            const auto p = std::get<PointLonLat>(laea->inv(PointXY{d, 0.}));
            EXPECT(north ? (89.9 < p.lat() && p.lat() < previous) : (previous < p.lat() && p.lat() < -89.9));
            EXPECT(points_equal(laea->fwd(p), PointXY{d, 0.}, 2e-3 * d));
            previous = p.lat();
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
