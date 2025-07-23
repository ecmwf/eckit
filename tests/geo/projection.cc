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

#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/projection/LonLatToXYZ.h"  // to test Reverse
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("projection: none") {
    Point p = PointLonLat{1, 1};
    std::unique_ptr<Projection> projection(ProjectionFactoryType::instance().get("none").create(spec::Custom{}));

    EXPECT(points_equal(p, projection->inv(p)));
    EXPECT(points_equal(p, projection->fwd(p)));
}


CASE("projection: reverse") {
    projection::LonLatToXYZ ab(new figure::UnitSphere);
    projection::Reverse<projection::LonLatToXYZ> ba(new figure::UnitSphere);

    PointLonLat p = NORTH_POLE;
    PointXYZ q{0., 0., 1.};

    ASSERT(points_equal(q, ab.fwd(p)));
    ASSERT(points_equal(p, ab.inv(q)));

    // ensure fwd(PointXYZ) -> PointLonLat, inv(PointLonLat) -> PointXYZ
    EXPECT(points_equal(p, ba.fwd(q)));
    EXPECT(points_equal(q, ba.inv(p)));

    ASSERT(ab.spec().get_string("type") == "ll-to-xyz");
    EXPECT(ba.spec().get_string("type") == "reverse-ll-to-xyz");
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
