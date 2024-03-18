/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iomanip>

#include "eckit/geometry/CoordinateHelpers.h"
#include "eckit/geometry/Point2.h"
#include "eckit/testing/Test.h"


namespace eckit::test {

using namespace geometry;

// -----------------------------------------------------------------------------

CASE("normalise angles") {
    EXPECT(0. == normalise_angle(360., 0.));
    EXPECT(14. == normalise_angle(374., 0.));
    EXPECT(14. == normalise_angle(374., -90.));
    EXPECT(374. == normalise_angle(374., 90.));
    EXPECT(14. == normalise_angle(-346., 0.));
    EXPECT(14. == normalise_angle(-346., -90.));
    EXPECT(374. == normalise_angle(-346., 90.));
    EXPECT(0. == normalise_angle(360. * 1e12, 0.));
    EXPECT(14. == normalise_angle(360. * 1e12 + 14, 0.));
    EXPECT(0. == normalise_angle(-360. * 1e12, 0.));
    EXPECT(14. == normalise_angle(-360. * 1e12 + 14, 0.));
}

CASE("normalise angles that should stay bit-identical") {

    std::vector<double> lats{
        0x1.a99999999999fp+3,  // 13.3
        0x1.7599999999999p+5,  // 46.7
        -0x1.37823af2187f7p+4, // -19.4692944962371
        0x1.14f26c8adc252p+3,  // 8.65459277268488
        0x1.237e9f537dd2dp+5,  // 36.4368273279928
        0x1.eb74b977e1e89p+5,  // 61.431994377691
        0x1.1008717af4f67p+6,  // 68.0082453929914
        -0x1.b4f88656270d9p+4, // -27.3106749883017
        -0x1.eb22f87f6ac12p+1, // -3.83700472089323
        0x1.40de11e0c3e99p+4,  // 20.0542162685293
        0x1.4aeba99be1331p+5,  // 41.3650695970631
        0x1.aa5c50f727ae6p+5,  // 53.2950763043389
        -0x1.556ccf04ef1bbp+4, // -21.3390646164641
        0x1.556ccf04ef1bbp+4,  // 21.3390646164641
        0x1.388f683df92bbp+5,  // 39.070023044745
        -0x1.40de11e0c3e9dp+4, // -20.0542162685293
        0x1.eb22f87f6abf5p+1,  // 3.83700472089321
        0x1.b4f88656270d7p+4,  // 27.3106749883017
    };
    for(const auto& lat: lats) {
        double normalised = normalise_angle(lat,-90.);
        // std::cout << std::hexfloat << lat << "\t : " << std::defaultfloat << std::setprecision(17) << lat << std::endl;
        // std::cout << std::hexfloat << normalised << "\t : " << std::defaultfloat << std::setprecision(17) << normalised << std::endl;
        EXPECT_EQUAL(normalised, lat);
    }
    std::vector<double> lons{
        -0x1.3f0f4411db559p+5, // -39.8824540515004
        -0x1.63664f7d2181dp+5, // -44.4249563003398
        -0x1.75e470fd085aap+5, // -46.7365436332869
        -0x1.b2a6314996231p+4, // -27.1655743479225
        -0x1.f720e2a9525edp+5, // -62.8910573223364
        -0x1.236723c039272p+5, // -36.425361158127
        -0x1.7f9f1a40a5d1fp+4, // -23.9763433957388
        0x1.ffffffffffffep+0,  // 2.0
        0x1.0b907154a92f7p+6,  // 66.8910573223364
        0x1.436723c039272p+5,  // 40.425361158127
        0x1.bf9f1a40a5d1fp+4,  // 27.9763433957388
        0x1.0f266c20b79f9p+7,  // 135.57504369966
        0x1.787bbbb54c676p+6,  // 94.1208332374461
        0x1.95e470fd085aap+5,  // 50.7365436332869
        0x1.1bd0dd7b42b69p+7,  // 141.907939769643
        0x1.19981bd70b549p+6,  // 70.3985437012353
        0x1.50bc8a12f525bp+5,  // 42.0920602303565
        0x1.cb2a2664f7bbdp+6,  // 114.791162088032
        0x1.6784444ab398ap+6,  // 89.8791667625539
        0x1.83664f7d2181ep+5,  // 48.4249563003398
        0x1.380c1cb7eb45dp+7,  // 156.023656604261
        0x1.d46f8eab56d0bp+6,  // 117.108942677664
        0x1.5f0f4411db559p+5,  // 43.8824540515004
    };
    for(const auto& lon: lons) {
        double normalised = normalise_angle(lon,-180.);
        // std::cout << std::hexfloat << lon << "\t : " << std::defaultfloat << std::setprecision(17) << lon << std::endl;
        // std::cout << std::hexfloat << normalised << "\t : " << std::defaultfloat << std::setprecision(17) << normalised << std::endl;
        EXPECT_EQUAL(normalised, lon);
    }
}

CASE("normalise angles that should be reproducible") {
    constexpr double minimum = -90.;
    std::vector<std::tuple<double,double>> reproduce{
        //         angle               normalised
        {0x1.a99999999999fp+3,      0x1.a99999999999fp+3},
        {0x1.a99999999999fp+3-360., 0x1.a9999999999ap+3 },
        {0x1.a99999999999fp+3-720., 0x1.a99999999998p+3 },
        {0x1.a99999999999fp+3+360., 0x1.a9999999999ap+3 },
        {0x1.a99999999999fp+3+720., 0x1.a99999999998p+3 },
    };
    for (const auto& [angle, normalised_ref] : reproduce) {
        double normalised = normalise_angle(angle, minimum);
        // std::cout << std::hexfloat << angle << "\t : " << std::defaultfloat << std::setprecision(17) << angle << std::endl;
        // std::cout << std::hexfloat << normalised << "\t : " << std::defaultfloat << std::setprecision(17) << normalised << std::endl;
        EXPECT(normalised == normalised_ref);
    }
}


CASE("canonicalise on sphere") {
    using types::is_approximately_equal;

    const Point2 p1(108., 32.);

    // Worse coordinates for the same point:
    const Point2 p2(-252., 32.);
    const Point2 p3(288., 148.);
    const Point2 p4(108., -328.);

    // Check each of these is correctly shifted back to original point:
    const Point2 q2 = canonicaliseOnSphere(p2);
    const Point2 q3 = canonicaliseOnSphere(p3);
    const Point2 q4 = canonicaliseOnSphere(p4);

    EXPECT(p1.x() == q2.x());
    EXPECT(p1.y() == q2.y());
    EXPECT(p1.x() == q3.x());
    EXPECT(p1.y() == q3.y());
    EXPECT(p1.x() == q4.x());
    EXPECT(p1.y() == q4.y());

    // Check with longitude offset
    const Point2 q2b = canonicaliseOnSphere(p2, -90.);
    EXPECT(q2b.x() == 108.);
    EXPECT(q2b.y() == 32.);

    const Point2 q2c = canonicaliseOnSphere(p2, 90.);
    EXPECT(q2c.x() == 108.);
    EXPECT(q2c.y() == 32.);

    const Point2 q2d = canonicaliseOnSphere(p2, 180.);
    EXPECT(q2d.x() == 468.);
    EXPECT(q2d.y() == 32.);
}

// -----------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
