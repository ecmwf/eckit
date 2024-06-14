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


CASE("projection: mercator") {
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


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
