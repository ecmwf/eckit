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


#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("PointLonLat normalise_angle_to_*") {
    struct test_t {
        double angle;
        double lim;
        double ref;
    };


    SECTION("normalise_angle_to_minimum") {
        for (const test_t& test : {
                 test_t{10., 0., 10.},
                 {0., 0., 0.},
                 {-10., 0., 350.},
                 {720., 0., 0.},
                 {100., 90., 100.},
                 {-370., 0., 350.},
                 {100000., 0., static_cast<double>(100000 % 360)},
                 {-100., -180., -100.},
                 {360., 0., 0.},
                 {100000., 99960., 100000.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLat::normalise_angle_to_minimum(test.angle, test.lim), PointLonLat::EPS));
        }
    }


    SECTION("normalise_angle_to_maximum") {
        for (const auto& test : {
                 test_t{350., 360., 350.},
                 {360., 360., 360.},
                 {361., 360., 1.},
                 {-720., 360., 360.},
                 {100., 180., 100.},
                 {-370., 360., 350.},
                 {100000., 360., static_cast<double>(100000 % 360)},
                 {-100., -90., -100.},
                 {720., 360., 360.},
                 {100040., 100080., 100040.},
             }) {
            EXPECT(types::is_approximately_equal(
                test.ref, PointLonLat::normalise_angle_to_maximum(test.angle, test.lim), PointLonLat::EPS));
        }
    }
}


CASE("PointLonLat antipode") {
    PointLonLat p(300., -10.);
    auto q = p.antipode();

    EXPECT(points_equal(q, {120., 10.}));
    EXPECT(points_equal(p, q.antipode()));

    PointLonLat r(-10., -91.);

    EXPECT(points_equal(r.antipode(), {350., 89.}));
    EXPECT(points_equal(r, r.antipode().antipode()));

    PointLonLat s(1., -90.);
    auto t = s.antipode();

    EXPECT_EQUAL(t.lon, 0.);
    EXPECT(points_equal(t, {2., 90.}));
    EXPECT(points_equal(t.antipode(), s));
}


#if eckit_HAVE_GEO_BITREPRODUCIBLE
CASE("bit-identical behaviour normalising angles") {
    auto normalise = [](double a, double minimum) -> double {
        auto modulo_360 = [](double a) { return a - 360. * std::floor(a / 360.); };
        auto diff       = a - minimum;
        return 0. <= diff && diff < 360. ? a : modulo_360(diff) + minimum;
    };

    struct test_t {
        double angle;
        double ref_norm_angle_m_360;
        double ref_norm_angle_m_720;
        double ref_norm_angle_p_360;
        double ref_norm_angle_p_720;
    };

    for (const auto& test : {
             test_t{0x1.a99999999999fp+3, 0x1.a9999999999ap+3, 0x1.a99999999998p+3, 0x1.a99999999998p+3,
                    0x1.a99999999998p+3},  // 13.30000000000001
             {0x1.7599999999999p+5, 0x1.7599999999998p+5, 0x1.75999999999ap+5, 0x1.75999999999ap+5,
              0x1.75999999999ap+5},  // 46.699999999999996
             {-0x1.37823af2187f7p+4, -0x1.37823af2187fp+4, -0x1.37823af2188p+4, -0x1.37823af2188p+4,
              -0x1.37823af2188p+4},  //-19.469294496237094
             {0x1.14f26c8adc252p+3, 0x1.14f26c8adc26p+3, 0x1.14f26c8adc24p+3, 0x1.14f26c8adc28p+3,
              0x1.14f26c8adc24p+3},  // 8.6545927726848824
             {0x1.237e9f537dd2dp+5, 0x1.237e9f537dd3p+5, 0x1.237e9f537dd3p+5, 0x1.237e9f537dd3p+5,
              0x1.237e9f537dd3p+5},  // 36.436827327992752
             {0x1.eb74b977e1e89p+5, 0x1.eb74b977e1e88p+5, 0x1.eb74b977e1e9p+5, 0x1.eb74b977e1e8p+5,
              0x1.eb74b977e1e9p+5},  // 61.431994377690962
             {0x1.1008717af4f67p+6, 0x1.1008717af4f68p+6, 0x1.1008717af4f68p+6, 0x1.1008717af4f68p+6,
              0x1.1008717af4f68p+6},  // 68.008245392991384
             {-0x1.b4f88656270d9p+4, -0x1.b4f88656270ep+4, -0x1.b4f88656270ep+4, -0x1.b4f88656270ep+4,
              -0x1.b4f88656270ep+4},  //-27.31067498830166
             {-0x1.eb22f87f6ac12p+1, -0x1.eb22f87f6acp+1, -0x1.eb22f87f6acp+1, -0x1.eb22f87f6acp+1,
              -0x1.eb22f87f6acp+1},  //-3.8370047208932272
             {0x1.40de11e0c3e99p+4, 0x1.40de11e0c3eap+4, 0x1.40de11e0c3eap+4, 0x1.40de11e0c3eap+4,
              0x1.40de11e0c3eap+4},  // 20.054216268529306
             {0x1.4aeba99be1331p+5, 0x1.4aeba99be133p+5, 0x1.4aeba99be133p+5, 0x1.4aeba99be133p+5,
              0x1.4aeba99be133p+5},  // 41.365069597063105
             {0x1.aa5c50f727ae6p+5, 0x1.aa5c50f727ae8p+5, 0x1.aa5c50f727aep+5, 0x1.aa5c50f727aep+5,
              0x1.aa5c50f727aep+5},  // 53.295076304338906
             {-0x1.556ccf04ef1bbp+4, -0x1.556ccf04ef1cp+4, -0x1.556ccf04ef1cp+4, -0x1.556ccf04ef1cp+4,
              -0x1.556ccf04ef1cp+4},  //-21.339064616464139
             {0x1.556ccf04ef1bbp+4, 0x1.556ccf04ef1cp+4, 0x1.556ccf04ef1cp+4, 0x1.556ccf04ef1cp+4,
              0x1.556ccf04ef1cp+4},  // 21.339064616464139
             {0x1.388f683df92bbp+5, 0x1.388f683df92b8p+5, 0x1.388f683df92cp+5, 0x1.388f683df92cp+5,
              0x1.388f683df92cp+5},  // 39.070023044745049
             {-0x1.40de11e0c3e9dp+4, -0x1.40de11e0c3eap+4, -0x1.40de11e0c3eap+4, -0x1.40de11e0c3eap+4,
              -0x1.40de11e0c3eap+4},  //-20.05421626852932
             {0x1.eb22f87f6abf5p+1, 0x1.eb22f87f6acp+1, 0x1.eb22f87f6acp+1, 0x1.eb22f87f6acp+1,
              0x1.eb22f87f6acp+1},  // 3.8370047208932143
             {0x1.b4f88656270d7p+4, 0x1.b4f88656270dp+4, 0x1.b4f88656270ep+4, 0x1.b4f88656270cp+4,
              0x1.b4f88656270ep+4},  // 27.310674988301653
             {-0x1.3f0f4411db559p+5, -0x1.3f0f4411db558p+5, -0x1.3f0f4411db56p+5, -0x1.3f0f4411db558p+5,
              -0x1.3f0f4411db56p+5},  //-39.882454051500368
             {-0x1.63664f7d2181dp+5, -0x1.63664f7d2182p+5, -0x1.63664f7d2182p+5, -0x1.63664f7d2182p+5,
              -0x1.63664f7d2182p+5},  //-44.424956300339751
             {-0x1.75e470fd085aap+5, -0x1.75e470fd085a8p+5, -0x1.75e470fd085bp+5, -0x1.75e470fd085a8p+5,
              -0x1.75e470fd085bp+5},  //-46.7365436332869
             {-0x1.b2a6314996231p+4, -0x1.b2a631499623p+4, -0x1.b2a631499624p+4, -0x1.b2a631499624p+4,
              -0x1.b2a631499624p+4},  //-27.165574347922476
             {-0x1.f720e2a9525edp+5, -0x1.f720e2a9525fp+5, -0x1.f720e2a9525fp+5, -0x1.f720e2a9525fp+5,
              -0x1.f720e2a9525fp+5},  //-62.89105732233643
             {-0x1.236723c039272p+5, -0x1.236723c03927p+5, -0x1.236723c03927p+5, -0x1.236723c03927p+5,
              -0x1.236723c03927p+5},  //-36.425361158126989
             {-0x1.7f9f1a40a5d1fp+4, -0x1.7f9f1a40a5d2p+4, -0x1.7f9f1a40a5d2p+4, -0x1.7f9f1a40a5d2p+4,
              -0x1.7f9f1a40a5d2p+4},                                  //-23.976343395738805
             {0x1.ffffffffffffep+0, 0x1p+1, 0x1p+1, 0x1p+1, 0x1p+1},  // 1.9999999999999996
             {0x1.0b907154a92f7p+6, 0x1.0b907154a92f8p+6, 0x1.0b907154a92f8p+6, 0x1.0b907154a92f8p+6,
              0x1.0b907154a92f8p+6},  // 66.891057322336437
             {0x1.436723c039272p+5, 0x1.436723c03927p+5, 0x1.436723c03927p+5, 0x1.436723c03927p+5,
              0x1.436723c03927p+5},  // 40.425361158126989
             {0x1.bf9f1a40a5d1fp+4, 0x1.bf9f1a40a5d2p+4, 0x1.bf9f1a40a5d2p+4, 0x1.bf9f1a40a5d2p+4,
              0x1.bf9f1a40a5d2p+4},  // 27.976343395738805
             {0x1.0f266c20b79f9p+7, 0x1.0f266c20b79f8p+7, 0x1.0f266c20b79f8p+7, 0x1.0f266c20b79f8p+7,
              0x1.0f266c20b79f8p+7},  // 135.57504369966026
             {0x1.787bbbb54c676p+6, 0x1.787bbbb54c678p+6, 0x1.787bbbb54c678p+6, 0x1.787bbbb54c678p+6,
              0x1.787bbbb54c678p+6},  // 94.120833237446135
             {0x1.95e470fd085aap+5, 0x1.95e470fd085a8p+5, 0x1.95e470fd085bp+5, 0x1.95e470fd085ap+5,
              0x1.95e470fd085bp+5},  // 50.7365436332869
             {0x1.1bd0dd7b42b69p+7, 0x1.1bd0dd7b42b68p+7, 0x1.1bd0dd7b42b68p+7, 0x1.1bd0dd7b42b68p+7,
              0x1.1bd0dd7b42b68p+7},  // 141.90793976964349
             {0x1.19981bd70b549p+6, 0x1.19981bd70b548p+6, 0x1.19981bd70b548p+6, 0x1.19981bd70b548p+6,
              0x1.19981bd70b548p+6},  // 70.39854370123534
             {0x1.50bc8a12f525bp+5, 0x1.50bc8a12f5258p+5, 0x1.50bc8a12f526p+5, 0x1.50bc8a12f526p+5,
              0x1.50bc8a12f526p+5},  // 42.092060230356502
             {0x1.cb2a2664f7bbdp+6, 0x1.cb2a2664f7bbcp+6, 0x1.cb2a2664f7bcp+6, 0x1.cb2a2664f7bcp+6,
              0x1.cb2a2664f7bcp+6},  // 114.79116208803221
             {0x1.6784444ab398ap+6, 0x1.6784444ab3988p+6, 0x1.6784444ab3988p+6, 0x1.6784444ab3988p+6,
              0x1.6784444ab3988p+6},  // 89.879166762553865
             {0x1.83664f7d2181ep+5, 0x1.83664f7d2182p+5, 0x1.83664f7d2182p+5, 0x1.83664f7d2182p+5,
              0x1.83664f7d2182p+5},  // 48.424956300339758
             {0x1.380c1cb7eb45dp+7, 0x1.380c1cb7eb45cp+7, 0x1.380c1cb7eb45cp+7, 0x1.380c1cb7eb45cp+7,
              0x1.380c1cb7eb46p+7},  // 156.02365660426122
             {0x1.d46f8eab56d0bp+6, 0x1.d46f8eab56d0cp+6, 0x1.d46f8eab56d08p+6, 0x1.d46f8eab56d1p+6,
              0x1.d46f8eab56d08p+6},  // 117.10894267766359
             {0x1.5f0f4411db559p+5, 0x1.5f0f4411db558p+5, 0x1.5f0f4411db56p+5, 0x1.5f0f4411db56p+5,
              0x1.5f0f4411db56p+5},  // 43.882454051500368
         }) {
        EXPECT(test.angle == normalise(test.angle, -180.));
        EXPECT(test.ref_norm_angle_m_360 == normalise(test.angle - 360., -180.));
        EXPECT(test.ref_norm_angle_m_720 == normalise(test.angle - 720., -180.));
        EXPECT(test.ref_norm_angle_p_360 == normalise(test.angle + 360., -180.));
        EXPECT(test.ref_norm_angle_p_720 == normalise(test.angle + 720., -180.));
    }
}
#endif


CASE("PointLonLat normalisation") {
    constexpr auto da = 1e-3;

    for (double a : {-370., -190., -100., -90., -80., -10., 0., 10., 80., 90., 100., 190., 370.}) {
        EXPECT(types::is_approximately_equal(a, PointLonLat::normalise_angle_to_minimum(a, a), PointLonLat::EPS));
        EXPECT(types::is_approximately_equal(a, PointLonLat::normalise_angle_to_maximum(a, a), PointLonLat::EPS));

        EXPECT(types::is_approximately_equal(a + 360., PointLonLat::normalise_angle_to_minimum(a - da, a) + da,
                                             PointLonLat::EPS));
        EXPECT(types::is_approximately_equal(a - 360., PointLonLat::normalise_angle_to_maximum(a + da, a) - da,
                                             PointLonLat::EPS));
    }

    PointLonLat p(1, 90.);
    EXPECT_EQUAL(p.lon, 1.);
    EXPECT_EQUAL(p.lat, 90.);

    auto p2 = PointLonLat::make(p.lon, p.lat);
    EXPECT_EQUAL(p2.lon, 0.);
    EXPECT(points_equal(p, p2));

    auto p3 = PointLonLat(50., 90.);
    EXPECT(points_equal(p, p3));
}


CASE("PointLonLat comparison") {
    Point a1 = PointLonLat{180., 0.};
    Point a2 = PointLonLat{-180., 0.};
    EXPECT(points_equal(a1, a2));

    Point b1 = PointLonLat{0., -90.};
    Point b2 = PointLonLat::make(1., 270.);
    EXPECT(points_equal(b1, b2));

    Point c1 = PointLonLat{300, -30};
    Point c2 = PointLonLat{-59.99999999999996, -30.000000000000018};
    EXPECT(points_equal(c1, c2));

    Point d1 = PointLonLat{-178., -46.7};
    Point d2 = PointLonLat{-178.00000000000003, -46.7};
    EXPECT(points_equal(d1, d2));

    Point e1 = PointLonLat(0., 90.);
    Point e2 = PointLonLat(180., 90.);
    EXPECT(points_equal(e1, e2));

    Point f1 = PointLonLat(-0., -90.);
    Point f2 = PointLonLat(-180., -90.);
    EXPECT(points_equal(f1, f2));
}


CASE("PointLonLat normalise angles") {
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(360., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(374., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(374., -90.));
    EXPECT(374. == PointLonLat::normalise_angle_to_minimum(374., 90.));

    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-346., 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-346., -90.));
    EXPECT(374. == PointLonLat::normalise_angle_to_minimum(-346., 90.));
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(360. * 1e12, 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(360. * 1e12 + 14, 0.));
    EXPECT(0. == PointLonLat::normalise_angle_to_minimum(-360. * 1e12, 0.));
    EXPECT(14. == PointLonLat::normalise_angle_to_minimum(-360. * 1e12 + 14, 0.));
}


CASE("PointLonLat canonicalise on sphere") {
    const PointLonLat p1(108., 32.);

    // Worse coordinates for the same point:
    const auto p2 = PointLonLat::make(-252., 32.);
    const auto p3 = PointLonLat::make(288., 148.);
    const auto p4 = PointLonLat::make(108., -328.);

    // Check each of these is correctly shifted back to original point:
    const PointLonLat q2 = PointLonLat::make(p2.lon, p2.lat);
    const PointLonLat q3 = PointLonLat::make(p3.lon, p3.lat);
    const PointLonLat q4 = PointLonLat::make(p4.lon, p4.lat);

    EXPECT(p1.lon == q2.lon);
    EXPECT(p1.lat == q2.lat);
    EXPECT(p1.lon == q3.lon);
    EXPECT(p1.lat == q3.lat);
    EXPECT(p1.lon == q4.lon);
    EXPECT(p1.lat == q4.lat);

    // Check with longitude offset
    EXPECT(points_equal(PointLonLat::make(1., -90.), PointLonLat(0., -90.)));
    EXPECT(points_equal(PointLonLat::make(2., 90.), PointLonLat(0., 90.)));
    EXPECT(points_equal(PointLonLat::make(3., 180.), PointLonLat(183., 0.)));

    // Check with latitude offset
    constexpr double eps = 1.e-6;

    EXPECT(points_equal(PointLonLat::make(-1., 89.99999914622634, 0., eps), {0., 90.}));
    EXPECT(points_equal(PointLonLat::make(1., -89.99999914622634, 0., eps), {0., -90.}));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
