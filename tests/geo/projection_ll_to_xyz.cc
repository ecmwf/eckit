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

#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("projection: ll_to_xyz") {
    struct P : std::unique_ptr<Projection> {
        explicit P(Projection* ptr) : unique_ptr(ptr) { ASSERT(unique_ptr::operator bool()); }
    };

    struct test_t {
        PointLonLat a;
        Point3 b;
    };

    constexpr double R = 1.;
    const auto L       = R * std::sqrt(2.) / 2.;


    // spherical projections
    P to_xyz_1(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom{{"R", 1.}}));
    P to_xyz_2(new projection::LonLatToXYZ(1., 1.));

    EXPECT(*to_xyz_1 == *to_xyz_2);
    EXPECT(*to_xyz_1 == projection::LonLatToXYZ(1.));


    // oblate spheroid projections
    P to_xyz_3(ProjectionFactory::instance().get("ll_to_xyz").create(spec::Custom{{"a", 1.}, {"b", 0.5}}));
    P to_xyz_4(new projection::LonLatToXYZ(1., 0.5));

    EXPECT(*to_xyz_3 == *to_xyz_4);


    // problate spheroid (not supported)
    EXPECT_THROWS(projection::LonLatToXYZ(0.5, 1.));


    SECTION("spec") {
        Log::info() << to_xyz_1->spec_str() << std::endl;
        Log::info() << to_xyz_2->spec_str() << std::endl;
        Log::info() << to_xyz_3->spec_str() << std::endl;
        Log::info() << to_xyz_4->spec_str() << std::endl;
        EXPECT(to_xyz_1->spec_str() == R"({"projection":"ll_to_xyz","r":1})");
        EXPECT(to_xyz_2->spec_str() == R"({"projection":"ll_to_xyz","r":1})");
        EXPECT(to_xyz_3->spec_str() == R"({"a":1,"b":0.5,"projection":"ll_to_xyz"})");
        EXPECT(to_xyz_4->spec_str() == R"({"a":1,"b":0.5,"projection":"ll_to_xyz"})");
    }


    SECTION("roundtrip") {
        for (const auto& p : {
                 PointLonLat{1, 1},
                 PointLonLat{1, 0},
                 PointLonLat(723., 1.),
             }) {
            EXPECT(points_equal(p, to_xyz_1->inv(to_xyz_1->fwd(p))));
            EXPECT(points_equal(p, to_xyz_2->inv(to_xyz_2->fwd(p))));

            EXPECT(points_equal(to_xyz_1->fwd(p), to_xyz_2->fwd(p)));
            EXPECT(points_equal(to_xyz_2->fwd(p), to_xyz_1->fwd(p)));

            if (p.lat == 0) {
                auto q = to_xyz_3->fwd(p);
                EXPECT(points_equal(to_xyz_1->fwd(p), q));
                EXPECT(points_equal(to_xyz_2->inv(q), p));
            }
        }
    }


    SECTION("sphere (ll -> xyz, xyz -> ll)") {
        for (const auto& test : {
                 test_t{{0, 90}, {0, 0, R}},  //
                 {{0, -90}, {0, 0, -R}},      //
                 {{0, 0}, {R, 0, 0}},         //
                 {{-360, 0}, {R, 0, 0}},      //
                 {{90, 0}, {0, R, 0}},        //
                 {{-270, 0}, {0, R, 0}},      //
                 {{180, 0}, {-R, 0, 0}},      //
                 {{-180, 0}, {-R, 0, 0}},     //
                 {{270, 0}, {0, -R, 0}},      //
                 {{-90, 0}, {0, -R, 0}},      //
                 {{45, 0}, {L, L, 0}},        //
                 {{-315, 0}, {L, L, 0}},      //
                 {{135, 0}, {-L, L, 0}},      //
                 {{-225, 0}, {-L, L, 0}},     //
                 {{225, 0}, {-L, -L, 0}},     //
                 {{-135, 0}, {-L, -L, 0}},    //
                 {{315, 0}, {L, -L, 0}},      //
                 {{-45, 0}, {L, -L, 0}},      //
             }) {
            EXPECT(points_equal(to_xyz_1->fwd(test.a), test.b));
            EXPECT(points_equal(to_xyz_1->inv(test.b), test.a));

            EXPECT(points_equal(to_xyz_2->fwd(test.a), test.b));
            EXPECT(points_equal(to_xyz_2->inv(test.b), test.a));
        }
    }


    SECTION("spheroid (ll -> xyz)") {
        for (const auto& test : {
                 test_t{{0, -90}, {0, 0, -0.5}},  //
                 {{42, -90}, {0, 0, -0.5}},       //
                 {{0, 90}, {0, 0, 0.5}},          //
                 {{42, 90}, {0, 0, 0.5}},         //
             }) {
            EXPECT(points_equal(to_xyz_3->fwd(test.a), test.b));
            EXPECT(points_equal(to_xyz_4->fwd(test.a), test.b));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
