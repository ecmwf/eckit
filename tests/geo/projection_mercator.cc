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


#include <memory>

#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/Mercator.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


CASE("projection: mercator") {
    PointLonLat first{262.036, 14.7365};


    SECTION("mercator (1)") {
        projection::Mercator projection(0., 14., new figure::Sphere(6371229.), first);

        Point2 a{0., 0.};
        auto b = projection.inv(a);
        auto c = projection.fwd(b);

        EXPECT(points_equal(c, a));
    }


    SECTION("mercator (2)") {
        projection::Mercator projection(-180., 0., new figure::Sphere(1.), {0, 0});

        PointLonLat a{-75., 35.};
        auto b = projection.fwd({-75, 35});
        auto c = projection.inv(b);

        EXPECT(points_equal(c, a));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
