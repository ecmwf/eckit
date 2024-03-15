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

#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


CASE("projection: ll_to_xyz") {
    Point p = PointLonLat{1, 1};
    P s1(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"R", 1.}})));
    P s2(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 1.}})));
    P s3(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 0.5}})));

    EXPECT(points_equal(p, s1->inv(s1->fwd(p))));
    EXPECT(points_equal(p, s2->inv(s2->fwd(p))));
    EXPECT(points_equal(s1->fwd(p), s2->fwd(p)));

    Point q = PointLonLat{1, 0};

    EXPECT(points_equal(s1->fwd(q), s3->fwd(q)));
    EXPECT(points_equal(s2->fwd(q), s3->fwd(q)));

    struct {
        PointLonLat a;
        Point3 b;
    } tests[] = {
        {{0, 0}, {1, 0, 0}},
        {{90, 0}, {0, 1, 0}},
        {{180, 0}, {-1, 0, 0}},
        {{270, 0}, {0, -1, 0}},
        {{0, -90}, {0, 0, -0.5}},
        {{42, -90}, {0, 0, -0.5}},
        {{0, 90}, {0, 0, 0.5}},
        {{42, 90}, {0, 0, 0.5}},
    };

    for (const auto& test : tests) {
        EXPECT(points_equal(s3->fwd(test.a), test.b));
    }
}


CASE("projection: ll_to_xyz") {
    const PointLonLat p(723., 1.);  // <- FIXME

    projection::LonLatToXYZ to_xyz_r(1.);

    auto q = to_xyz_r.fwd(p);
    auto r = to_xyz_r.inv(q);
    Log::info() << "p(lat, lon): " << p << " -> p(x,y,z): " << q << " -> p(lat, lon): " << r << std::endl;

    EXPECT(points_equal(p, r));

    // oblate spheroid (supported)
    projection::LonLatToXYZ to_xyz_ab(3., 2.);

    for (const auto& lon : {0., 90., 180., 270.}) {
        PointLonLat p{lon, 0.};
        Log::info() << "p(lat, lon): " << p << " -> p_ab(x,y,z): " << to_xyz_ab.fwd(p) << std::endl;
    }

    // problate spheroid (not supported)
    EXPECT_THROWS(projection::LonLatToXYZ(2., 3.));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
