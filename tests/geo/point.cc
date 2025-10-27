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


#include <utility>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Point comparison") {
    auto r(PointLonLat::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    for (Point a1 : {PointLonLat{-180., 0.}, PointLonLat{180., 10.}, PointLonLat{0., 90.}, PointLonLat{1., -90.}}) {
        auto a2 = PointLonLat::make(std::get<PointLonLat>(a1).lon + 720., std::get<PointLonLat>(a1).lat);
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


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
