// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/AlbersEqualArea.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("AlbersEqualArea") {
    constexpr auto eps = 10. * PointXY::EPS;  // FIXME improve floating-point errors

    projection::AlbersEqualArea proj1(-96., 23., 29.5, 45.5, new figure::Sphere(6378206.4));
    std::unique_ptr<const Projection> proj2(
        ProjectionFactory::has_type("proj")
            ? ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"proj", proj1.proj_str()}})
            : nullptr);


    SECTION("proj_str") {
        EXPECT(proj1.proj_str() == R"(+proj=aea +lat_0=23 +lat_1=29.5 +lat_2=45.5 +lon_0=-96 +R=6378206.4)");
    }


    SECTION("inv(fwd(.)) == . and fwd(inv(.)) == .") {
        auto a   = PointXY{0., 0.};
        auto aa  = proj1.inv(a);
        auto aaa = proj1.fwd(aa);

        EXPECT(points_equal(a, aaa, eps));

        auto b   = PointLonLat{-75., 35.};
        auto bb  = proj1.fwd(b);
        auto bbb = proj1.inv(bb);

        EXPECT(points_equal(b, bbb));

        if (proj2) {
            auto aaaa  = std::get<PointLonLat>(proj2->inv(a));
            auto aaaaa = std::get<PointXY>(proj2->fwd(aaaa));

            EXPECT(points_equal(aa, aaaa));
            EXPECT(points_equal(a, aaaaa, eps));

            auto bbbb  = std::get<PointXY>(proj2->fwd(b));
            auto bbbbb = std::get<PointLonLat>(proj2->inv(bbbb));

            // EXPECT(points_equal(bb, bbbb, eps));
            EXPECT(points_equal(b, bbbbb));
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
