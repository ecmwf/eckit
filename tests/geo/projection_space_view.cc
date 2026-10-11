// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
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


constexpr double TOLERANCE = 1e-6;


const std::vector<std::string> SPECS{
    "{type: space_view, h: 35785831}",
    "{type: space_view, h: 35785831, lon_0: -75, sweep: x, figure: wgs84}",
    "{type: space_view, h: 35785831, lon_0: 9.5, figure: wgs84}",
};


// visible from the sub-satellite point (lon_0, 0)
std::vector<PointLonLat> points(double lon_0) {
    return {{lon_0, 0.}, {lon_0 + 30., 40.}, {lon_0 - 60., -20.}};
}


CASE("space_view: spec") {
    for (const auto& [spec, spec_str] : std::vector<std::pair<std::string, std::string>>{
             {SPECS[0], R"({"h":35785831,"type":"space_view"})"},
             {SPECS[1], R"({"figure":"wgs84","h":35785831,"lon_0":-75,"sweep":"x","type":"space_view"})"},
             {SPECS[2], R"({"figure":"wgs84","h":35785831,"lon_0":9.5,"type":"space_view"})"},
         }) {
        P projection(ProjectionFactory::make_from_string(spec));
        EXPECT_EQUAL(projection->spec_str(), spec_str);

        P same(ProjectionFactory::build(projection->spec()));
        EXPECT(*same == *projection);

        // not a PROJ projection
        EXPECT_THROWS(projection->proj_str());
    }
}


CASE("space_view: fwd, inv") {
    for (const auto& spec : SPECS) {
        P sv(ProjectionFactory::make_from_string(spec));
        const auto lon_0 = sv->spec().get_double("lon_0", 0.);

        EXPECT(points_equal(sv->fwd(PointLonLat{lon_0, 0.}), PointXY{0., 0.}, TOLERANCE));

        for (const auto& p : points(lon_0)) {
            EXPECT(points_equal(sv->inv(sv->fwd(p)), p, TOLERANCE));
        }

        // not visible
        EXPECT(std::isnan(std::get<PointXY>(sv->fwd(PointLonLat{lon_0 + 180., 0.})).X()));
        EXPECT(std::isnan(std::get<PointLonLat>(sv->inv(PointXY{1e8, 1e8})).lon()));
    }
}


CASE("space_view: compared to PROJ geos") {
    if (!ProjectionFactory::has_type("proj") || !LibEcKitGeo::projdb_is_available()) {
        return;
    }

    for (const auto& [spec, geos] : std::vector<std::pair<std::string, std::string>>{
             {SPECS[0], "+proj=geos +h=35785831 +R=6371229"},
             {SPECS[1], "+proj=geos +h=35785831 +lon_0=-75 +sweep=x +ellps=WGS84"},
             {SPECS[2], "+proj=geos +h=35785831 +lon_0=9.5 +ellps=WGS84"},
         }) {
        P sv(ProjectionFactory::make_from_string(spec));
        P proj(ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", geos}}));

        for (const auto& p : points(sv->spec().get_double("lon_0", 0.))) {
            EXPECT(points_equal(proj->inv(sv->fwd(p)), p, TOLERANCE));
            EXPECT(points_equal(sv->inv(proj->fwd(p)), p, TOLERANCE));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
