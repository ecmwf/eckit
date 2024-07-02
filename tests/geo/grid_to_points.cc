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
#include <vector>

#include "eckit/geo/grid/HEALPix.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("HEALPix") {
    SECTION("HEALPix::to_points") {
        std::unique_ptr<const Grid> grid(new grid::HEALPix(2, Ordering::healpix_ring));

        static const std::vector<PointLonLat> expected_points_ring{
            {45, 66.443535691},
            {135, 66.443535691},
            {225, 66.443535691},
            {315, 66.443535691},
            {22.5, 41.810314896},
            {67.5, 41.810314896},
            {112.5, 41.810314896},
            {157.5, 41.810314896},
            {202.5, 41.810314896},
            {247.5, 41.810314896},
            {292.5, 41.810314896},
            {337.5, 41.810314896},
            {0, 19.471220634},
            {45, 19.471220634},
            {90, 19.471220634},
            {135, 19.471220634},
            {180, 19.471220634},
            {225, 19.471220634},
            {270, 19.471220634},
            {315, 19.471220634},
            {22.5, 0},
            {67.5, 0},
            {112.5, 0},
            {157.5, 0},
            {202.5, 0},
            {247.5, 0},
            {292.5, 0},
            {337.5, 0},
            {0, -19.471220634},
            {45, -19.471220634},
            {90, -19.471220634},
            {135, -19.471220634},
            {180, -19.471220634},
            {225, -19.471220634},
            {270, -19.471220634},
            {315, -19.471220634},
            {22.5, -41.810314896},
            {67.5, -41.810314896},
            {112.5, -41.810314896},
            {157.5, -41.810314896},
            {202.5, -41.810314896},
            {247.5, -41.810314896},
            {292.5, -41.810314896},
            {337.5, -41.810314896},
            {45, -66.443535691},
            {135, -66.443535691},
            {225, -66.443535691},
            {315, -66.443535691},
        };

        auto points = grid->to_points();
        EXPECT_EQUAL(points.size(), expected_points_ring.size());

        for (int i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(std::get<PointLonLat>(points[i]), expected_points_ring[i]));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
