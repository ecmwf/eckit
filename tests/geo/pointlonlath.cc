// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sstream>

#include "eckit/geo/Point.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("PointLonLatH") {
    const PointLonLatH p{10., 20., 30.};

    EXPECT(p.lon() == 10.);
    EXPECT(p.lat() == 20.);
    EXPECT(p.height() == 30.);
    EXPECT(points_equal(p.lonlat(), PointLonLat{10., 20.}));
    EXPECT(PointLonLatH::dimensions() == 3);

    EXPECT(points_equal(p, PointLonLatH{PointLonLat{10., 20.}, 30.}));
    EXPECT_NOT(points_equal(p, PointLonLatH{10., 20., 31.}));
    EXPECT_NOT(points_equal(p, PointLonLatH{11., 20., 30.}));

    std::ostringstream str;
    str << p;
    EXPECT(str.str() == "{10, 20, 30}");
}


CASE("PointLonLatH make") {
    // (lon, lat) as PointLonLat::make, height kept
    EXPECT(points_equal(PointLonLatH::make(370., 20., 30.), PointLonLatH{10., 20., 30.}));
    EXPECT(points_equal(PointLonLatH::make(10., 100., -5.), PointLonLatH{190., 80., -5.}));
    EXPECT(points_equal(PointLonLatH::make(42., 90., 1.), PointLonLatH{NORTH_POLE, 1.}));

    // equal (lon, lat) on different longitude ranges
    EXPECT(points_equal(PointLonLatH{-10., 20., 30.}, PointLonLatH{350., 20., 30.}));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
