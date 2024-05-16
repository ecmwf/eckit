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

#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/figure/Mercator.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("projection: mercator (poles)") {
    projection::figure::Mercator projection({0., 14.}, {0., 0.}, new figure::Sphere(1.));

    auto a = projection.fwd(PointLonLat{0., 90.});
    EXPECT(a.Y > std::numeric_limits<double>::max());

    auto b = projection.fwd(PointLonLat{0., -90.});
    EXPECT(b.Y < std::numeric_limits<double>::lowest());
}


CASE("projection: mercator (1)") {
    projection::figure::Mercator projection({0., 14.}, {262.036, 14.7365}, new figure::Sphere(6371229.));

    Point2 a{0., 0.};
    auto b = projection.inv(a);
    auto c = projection.fwd(b);

    EXPECT(points_equal(c, a));
}


CASE("projection: mercator (2)") {
    projection::figure::Mercator projection({-180., 0.}, {0., 0.}, new figure::Sphere(1.));

    PointLonLat a{-75., 35.};
    auto b = projection.fwd(a);
    auto c = projection.inv(b);

    EXPECT(points_equal(c, a));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
