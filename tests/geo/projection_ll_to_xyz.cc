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


#include <cmath>
#include <memory>

#include "eckit/geo/projection/figure/LonLatToXYZ.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("projection: ll_to_xyz") {
    using P = std::unique_ptr<Projection>;

    constexpr double R = 1.;
    const auto L       = R * std::sqrt(2.) / 2.;

    struct test_t {
        PointLonLat a;
        Point3 b;
    };

    test_t test_sphere[] = {
        {{0, 0}, {R, 0, 0}},     //
        {{90, 0}, {0, R, 0}},    //
        {{180, 0}, {-R, 0, 0}},  //
        {{270, 0}, {0, -R, 0}},  //
    };

    test_t test_oblate_spheroid[] = {
        {{0, -90}, {0, 0, -0.5}},   //
        {{42, -90}, {0, 0, -0.5}},  //
        {{0, 90}, {0, 0, 0.5}},     //
        {{42, 90}, {0, 0, 0.5}},    //
    };


    SECTION("oblate/prolate spheroid") {
        // oblate spheroid (supported)
        EXPECT(P(new projection::figure::LonLatToXYZ(1., 0.5)));

        // problate spheroid (not supported)
        EXPECT_THROWS(projection::figure::LonLatToXYZ(0.5, 1.));
    }


    P to_xyz_1(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"R", 1.}})));
    P to_xyz_2(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 1.}})));
    P to_xyz_3(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom({{"a", 1.}, {"b", 0.5}})));


    SECTION("spec") {
        auto x = to_xyz_1->spec();
    }


    SECTION("") {
        Point p = PointLonLat{1, 1};

        EXPECT(points_equal(p, to_xyz_1->inv(to_xyz_1->fwd(p))));
        EXPECT(points_equal(p, to_xyz_2->inv(to_xyz_2->fwd(p))));
        EXPECT(points_equal(to_xyz_1->fwd(p), to_xyz_2->fwd(p)));

        Point q = PointLonLat{1, 0};

        EXPECT(points_equal(to_xyz_1->fwd(q), to_xyz_3->fwd(q)));
        EXPECT(points_equal(to_xyz_2->fwd(q), to_xyz_3->fwd(q)));

        for (const auto& test : test_sphere) {
            EXPECT(points_equal(to_xyz_1->fwd(test.a), test.b));
            EXPECT(points_equal(to_xyz_2->fwd(test.a), test.b));
            EXPECT(points_equal(to_xyz_3->fwd(test.a), test.b));
        }

        for (const auto& test : test_oblate_spheroid) {
            EXPECT(points_equal(to_xyz_3->fwd(test.a), test.b));
        }
    }


    SECTION("") {
        const PointLonLat p(723., 1.);  // <- FIXME

        auto q = to_xyz_1->fwd(p);
        auto r = to_xyz_1->inv(q);
        Log::info() << "p(lat, lon): " << p << " -> p(x,y,z): " << q << " -> p(lat, lon): " << r << std::endl;

        EXPECT(points_equal(p, r));

        for (const auto& lon : {0., 90., 180., 270.}) {
            PointLonLat p{lon, 0.};
            Log::info() << "p(lat, lon): " << p << " -> p_ab(x,y,z): " << to_xyz_3->fwd(p) << std::endl;
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
