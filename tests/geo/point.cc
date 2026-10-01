// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("points_equal") {
    auto r(PointLonLat::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    for (Point a1 : {PointLonLat{-180., 0.}, PointLonLat{180., 10.}, PointLonLat{0., 90.}, PointLonLat{1., -90.}}) {
        auto a2 = PointLonLat::make(std::get<PointLonLat>(a1).lon() + 720., std::get<PointLonLat>(a1).lat());
        EXPECT(points_equal(a1, a2));
    }

    PointXY p2{1., 2.};
    PointXYZ p3{1., 2., 3.};
    PointLonLat pll{4., 5.};

    for (const auto& ab : std::vector<std::pair<Point, Point>>{{p2, p2}, {p3, p3}, {pll, pll}}) {
        EXPECT(points_equal(ab.first, ab.second));
    }

    for (const auto& ab :
         std::vector<std::pair<Point, Point>>{{p2, p3}, {p2, pll}, {p3, p2}, {p3, pll}, {pll, p2}, {pll, p3}}) {
        EXPECT_THROWS_AS(points_equal(ab.first, ab.second), AssertionFailed);
    }
}


CASE("point_coordinates") {
    using names = std::vector<std::string>;

    EXPECT((point_coordinates<PointXY>() == names{"x", "y"}));
    EXPECT((point_coordinates<PointXYZ>() == names{"x", "y", "z"}));
    EXPECT((point_coordinates<PointLonLat>() == names{"longitude", "latitude"}));
    EXPECT((point_coordinates<PointLonLatR>() == names{"longitude_r", "latitude_r"}));

    // one name per coordinate
    EXPECT(point_coordinates<PointXY>().size() == PointXY::DIMS);
    EXPECT(point_coordinates<PointXYZ>().size() == PointXYZ::DIMS);
    EXPECT(point_coordinates<PointLonLat>().size() == PointLonLat::DIMS);
    EXPECT(point_coordinates<PointLonLatR>().size() == PointLonLatR::DIMS);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
