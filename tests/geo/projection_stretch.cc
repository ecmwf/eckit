// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/projection/Stretch.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


const std::vector<PointLonLat> POINTS{{0., 0.}, {10., 20.}, {-30., -60.}, {200., 89.}, {45., -89.}};


CASE("stretch: spec, reverse (by the inverse factor)") {
    std::unique_ptr<const Projection> stretch(ProjectionFactory::make_from_string("{type: stretch, factor: 0.5}"));
    std::unique_ptr<const Projection> reverse(
        ProjectionFactory::make_from_string("{type: stretch, factor: 0.5, reverse: true}"));
    std::unique_ptr<const Projection> inverse(ProjectionFactory::make_from_string("{type: stretch, factor: 2}"));

    EXPECT(stretch->spec_str() == R"({"factor":0.5,"type":"stretch"})");
    EXPECT(dynamic_cast<const projection::Stretch*>(reverse.get()) != nullptr);
    EXPECT(!reverse->reverse());
    EXPECT(*reverse == *inverse);

    for (const auto& a : POINTS) {
        const auto b = stretch->fwd(a);
        EXPECT(points_equal(stretch->inv(b), a));
        EXPECT(points_equal(reverse->fwd(b), a));
        EXPECT(points_equal(reverse->inv(a), b));
    }

    EXPECT_THROWS_AS(projection::Stretch(0.), exception::ProjectionError);
    EXPECT_THROWS_AS(projection::Stretch(-1.), exception::ProjectionError);
}


CASE("stretch: Cartesian points, as (lon, lat)") {
    const projection::Stretch stretch(0.3);

    for (const auto& p : POINTS) {
        const auto xyz = figure::UnitSphere::_convertSphericalToCartesian(p);

        const auto q = stretch.fwd(xyz);
        EXPECT(points_equal(figure::UnitSphere::_convertCartesianToSpherical(q), stretch.fwd(p)));
        EXPECT(points_equal(stretch.inv(q), xyz));

        // as Point (variant)
        EXPECT(points_equal(std::get<PointXYZ>(stretch.fwd(Point{xyz})), q));
        EXPECT(points_equal(std::get<PointLonLat>(stretch.fwd(Point{p})), stretch.fwd(p)));
    }
}


CASE("stretch: near the poles (reference values, at 50 significant digits)") {
    const projection::Stretch stretch(0.5773502691896258);

    for (const auto& [lat, ref] : std::vector<std::pair<double, double>>{{89.999999, 89.99999942264974},
                                                                         {-89.999999, -89.9999982679492},
                                                                         {89.99999999, 89.9999999942265},
                                                                         {-89.99999999, -89.9999999826795},
                                                                         {45., 63.10104894772739}}) {
        EXPECT(std::abs(stretch.fwd(PointLonLat{10., lat}).lat() - ref) < 1e-13);
        EXPECT(std::abs(stretch.inv(PointLonLat{10., ref}).lat() - lat) < 1e-13);
    }
}


CASE("stretch: equatorial latitude") {
    for (double lat : {-60., -30., 0., 10., 45., 80.}) {
        const projection::Stretch stretch(projection::Stretch::factor_from_equatorial_latitude(lat));

        // the equator to the given latitude, the poles fixed, longitudes unchanged
        EXPECT(points_equal(stretch.fwd(PointLonLat{30., 0.}), PointLonLat{30., lat}));
        EXPECT(points_equal(stretch.fwd(NORTH_POLE), NORTH_POLE));
        EXPECT(points_equal(stretch.fwd(SOUTH_POLE), SOUTH_POLE));
    }

    EXPECT_THROWS_AS(projection::Stretch::factor_from_equatorial_latitude(90.), exception::ProjectionError);
    EXPECT_THROWS_AS(projection::Stretch::factor_from_equatorial_latitude(-90.), exception::ProjectionError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
