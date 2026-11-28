// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/grid/unstructured/UnstructuredLLH.h"
#include "eckit/geo/projection/ToFromXYZ.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::unstructured::UnstructuredLLH;


const std::vector<double> LON{0., 90., -45., 10., 10.};
const std::vector<double> LAT{0., 0., 30., -60., -60.};
const std::vector<double> HEIGHT{0., 100., 1000., 10., 3000.};


CASE("unstructured_llh") {
    const UnstructuredLLH grid(LON, LAT, HEIGHT);

    EXPECT(grid.type() == "unstructured_llh");
    EXPECT(grid.size() == LON.size());

    size_t i = 0;
    for (const auto& p : grid) {
        EXPECT(points_equal(std::get<PointLonLatH>(p), PointLonLatH(LON[i], LAT[i], HEIGHT[i])));
        ++i;
    }

    const auto [lat, lon] = grid.to_latlons();
    EXPECT(lat == LAT);
    EXPECT(lon == LON);

    SECTION("to_xyz, heights along the normal of the figure") {
        const auto xyz = grid.to_xyz();
        const auto& f  = grid.figure();
        for (size_t j = 0; j < grid.size(); ++j) {
            const auto q = projection::ToFromXYZ::to_xyz(f.a(), f.b(), PointLonLat{LON[j], LAT[j]}, HEIGHT[j]);
            EXPECT(points_equal(PointXYZ{xyz[0][j], xyz[1][j], xyz[2][j]}, q));
        }
    }

    SECTION("spec, by uid while in use") {
        EXPECT(grid.spec_str() == R"({"type":"unstructured_llh","uid":")" + grid.uid() + R"("})");

        std::unique_ptr<const Grid> same(GridFactory::build(spec::Custom{{"uid", grid.uid()}}));
        EXPECT(*same == grid);
        EXPECT(points_equal(same->first_point(), grid.first_point()));

        std::unique_ptr<const Grid> given(GridFactory::build(
            spec::Custom{{"type", "unstructured_llh"}, {"longitudes", LON}, {"latitudes", LAT}, {"heights", HEIGHT}}));
        EXPECT(given->uid() == grid.uid());
    }

    SECTION("errors") {
        EXPECT_THROWS_AS(UnstructuredLLH(LON, LAT, {1., 2.}), exception::GridError);
        EXPECT_THROWS_AS(UnstructuredLLH(spec::Custom{{"uid", "00000000000000000000000000000000"}}),
                         exception::GridUnknownError);
    }
}


CASE("unstructured_llh: coordinates released with the last grid") {
    const std::vector<double> lon{1., 2.};
    const auto uid = UnstructuredLLH(lon, {3., 4.}, {5., 6.}).uid();

    EXPECT_THROWS_AS(UnstructuredLLH(spec::Custom{{"uid", uid}}), exception::GridUnknownError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
