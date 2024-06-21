/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <limits>

#include "eckit/geo/projection/Mercator.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Mercator: projection inv(fwd(.)) == . and fwd(inv(.)) == .") {
    for (const auto& projection : {
             projection::Mercator({0., 14.}, {262.036, 14.7365}),
             projection::Mercator({-180., 0.}, {0., 0.}),
             projection::Mercator({0., 14.}, {0., 0.}),
         }) {
        Point2 a{0., 0.};
        EXPECT(points_equal(a, projection.fwd(projection.inv(a))));

        PointLonLat b{-75., 35.};
        EXPECT(points_equal(b, projection.inv(projection.fwd(b))));

        Point2 c = projection.fwd(NORTH_POLE);
        EXPECT(c.Y > std::numeric_limits<double>::max());

        Point2 d = projection.fwd(SOUTH_POLE);
        EXPECT(d.Y < std::numeric_limits<double>::lowest());
    }
}


CASE("Mercator: spec_str, proj_str") {
    projection::Mercator proj1({0., 14.}, {262.036, 14.7365});

    EXPECT(proj1.proj_str() == "+proj=merc +lat_ts=14 +R=6371229");
    EXPECT(proj1.spec_str() == R"({"lat_ts":14,"projection":"mercator","r":6371229})");

    projection::Mercator proj2({-180., 0.}, {0., 0.});

    EXPECT(proj2.proj_str() == "+proj=merc +lon_0=-180 +R=6371229");
    EXPECT(proj2.spec_str() == R"({"lon_0":-180,"projection":"mercator","r":6371229})");

    projection::Mercator proj3({0., 14.}, {0., 0.});

    EXPECT(proj3.proj_str() == "+proj=merc +lat_ts=14 +R=6371229");
    EXPECT(proj3.spec_str() == R"({"lat_ts":14,"projection":"mercator","r":6371229})");
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
