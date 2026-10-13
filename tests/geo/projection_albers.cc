// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <vector>

#include "eckit/geo/LibEcKitGeo.h"
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

            EXPECT(points_equal(bb, bbbb, 1e-6));
            EXPECT(points_equal(b, bbbbb));
        }
    }


    SECTION("longitudes either side of the central meridian") {
        for (const auto& p : std::vector<PointLonLat>{{0., 30.}, {-170., 40.}, {100., 50.}, {-300., 35.}}) {
            const auto q = proj1.fwd(p);
            EXPECT(points_equal(proj1.fwd(PointLonLat{p.lon() + 360., p.lat()}), q, 1e-6));
            EXPECT(points_equal(proj1.inv(q), p));

            if (proj2) {
                EXPECT(points_equal(std::get<PointXY>(proj2->fwd(p)), q, 1e-6));
            }
        }
    }
}


CASE("AlbersEqualArea: ellipsoid") {
    // Snyder (1987), Map Projections: A Working Manual, p. 292, Clarke 1866 ellipsoid
    std::unique_ptr<const Projection> aea(ProjectionFactory::make_from_string(
        "{type: aea, lon_0: -96, lat_0: 23, lat_1: 29.5, lat_2: 45.5, figure: {a: 6378206.4, b: 6356583.8}}"));

    EXPECT(aea->proj_str() == "+proj=aea +lat_0=23 +lat_1=29.5 +lat_2=45.5 +lon_0=-96 +a=6378206.4 +b=6356583.8");

    const PointLonLat p{-75., 35.};
    const auto q = std::get<PointXY>(aea->fwd(p));
    EXPECT(points_equal(q, PointXY{1885472.7, 1535925.0}, 0.1));
    EXPECT(points_equal(aea->inv(q), p));

    // a tangent cone (one standard parallel), southern
    std::unique_ptr<const Projection> south(ProjectionFactory::make_from_string(
        "{type: aea, lon_0: 132, lat_0: 0, lat_1: -18, lat_2: -18, figure: grs80}"));

    for (const auto& r : std::vector<PointLonLat>{{132., 0.}, {112., -10.}, {154., -43.}, {120., -30.}}) {
        EXPECT(points_equal(south->inv(south->fwd(r)), r));
    }

    if (ProjectionFactory::has_type("proj") && LibEcKitGeo::projdb_is_available()) {
        for (const auto* proj : {aea.get(), south.get()}) {
            std::unique_ptr<const Projection> by_proj(
                ProjectionFactory::build(spec::Custom{{"type", "proj"}, {"target", proj->proj_str()}}));

            for (const auto& r : std::vector<PointLonLat>{{-75., 35.}, {-120., 50.}, {112., -10.}, {154., -43.}}) {
                EXPECT(points_equal(by_proj->fwd(r), proj->fwd(r), 1e-3));
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
