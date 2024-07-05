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


#include <cmath>

#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/geometry/SphereT.h"
#include "eckit/geo/geometry/UnitSphere.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("unit sphere") {
    using geometry::UnitSphere;

    const auto R = UnitSphere::radius();
    const auto L = R * std::sqrt(2) / 2.;

    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai


    SECTION("radius") {
        EXPECT(UnitSphere::radius() == 1.);
    }


    SECTION("north pole") {
        auto p = UnitSphere::convertSphericalToCartesian({0., 90.});

        EXPECT(p.X == 0);
        EXPECT(p.Y == 0);
        EXPECT(p.Z == R);
    }


    SECTION("south pole") {
        auto p = UnitSphere::convertSphericalToCartesian({0., -90.});

        EXPECT(p.X == 0);
        EXPECT(p.Y == 0);
        EXPECT(p.Z == -R);
    }


    SECTION("distances") {
        // Same points with added shifts
        auto P1b = PointLonLat::make(288.4, -33.);    // Valparaíso + longitude shift
        auto P2b = PointLonLat::make(301.8, 148.6);   // Shanghai + latitude/longitude shift
        auto P2c = PointLonLat::make(-58.2, -211.4);  // Shanghai + latitude/longitude shift

        auto d0 = UnitSphere::distance(P1, P2);
        auto d1 = UnitSphere::distance(P1b, P2);
        auto d2 = UnitSphere::distance(P1, P2b);
        auto d3 = UnitSphere::distance(P1, P2c);

        EXPECT(types::is_approximately_equal(d0, d1));
        EXPECT(types::is_approximately_equal(d0, d2));
        EXPECT(types::is_approximately_equal(d0, d3));
    }


    SECTION("area globe") {
        EXPECT(UnitSphere::area() == 4. * M_PI * R * R);
    }


    SECTION("area hemispheres") {
        auto area_hemisphere_north = UnitSphere::area({90., -180., 0., 180.});
        auto area_hemisphere_south = UnitSphere::area({0., -180., -90., 180.});

        EXPECT(area_hemisphere_north == 0.5 * UnitSphere::area());
        EXPECT(area_hemisphere_north == area_hemisphere_south);
    }


    SECTION("lon 0 (quadrant)") {
        auto p = UnitSphere::convertSphericalToCartesian({0., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-360., 0.});

        EXPECT(p.X == R);
        EXPECT(p.Y == 0);
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 90 (quadrant)") {
        auto p = UnitSphere::convertSphericalToCartesian({90., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-270., 0.});

        EXPECT(p.X == 0);
        EXPECT(p.Y == R);
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 180 (quadrant)") {
        auto p = UnitSphere::convertSphericalToCartesian({180., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-180., 0.});

        EXPECT(p.X == -R);
        EXPECT(p.Y == 0);
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 270 (quadrant)") {
        auto p = UnitSphere::convertSphericalToCartesian({270., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-90., 0.});

        EXPECT(p.X == 0);
        EXPECT(p.Y == -R);
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 45 (octant)") {
        auto p = UnitSphere::convertSphericalToCartesian({45., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-315., 0.});

        EXPECT(types::is_approximately_equal(p.X, L));
        EXPECT(types::is_approximately_equal(p.Y, L));
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 135 (octant)") {
        auto p = UnitSphere::convertSphericalToCartesian({135., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-225., 0.});

        EXPECT(types::is_approximately_equal(p.X, -L));
        EXPECT(types::is_approximately_equal(p.Y, L));
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 225 (octant)") {
        auto p = UnitSphere::convertSphericalToCartesian({225., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-135., 0.});

        EXPECT(types::is_approximately_equal(p.X, -L));
        EXPECT(types::is_approximately_equal(p.Y, -L));
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lon 315 (octant)") {
        auto p = UnitSphere::convertSphericalToCartesian({315., 0.});
        auto q = UnitSphere::convertSphericalToCartesian({-45., 0.});

        EXPECT(types::is_approximately_equal(p.X, L));
        EXPECT(types::is_approximately_equal(p.Y, -L));
        EXPECT(p.Z == 0);

        EXPECT(p == q);
    }


    SECTION("lat 100") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(0., 100.)), BadValue);

        auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(0., 100.), 0.);
        auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(180., 80.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X, q.X));
        EXPECT(types::is_approximately_equal(p.Y, q.Y));
        EXPECT(types::is_approximately_equal(p.Z, q.Z));
    }


    SECTION("lat 290") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(15., 290.)), BadValue);

        auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(15., 290.), 0.);
        auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(15., -70.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X, q.X));
        EXPECT(types::is_approximately_equal(p.Y, q.Y));
        EXPECT(types::is_approximately_equal(p.Z, q.Z));
    }


    SECTION("lat -120") {
        // Default behavior throws
        EXPECT_THROWS_AS(PointLonLat::assert_latitude_range(PointLonLat(45., -120.)), BadValue);

        auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(45., -120.), 0.);
        auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(225., -60.), 0.);

        // sin(x) and sin(pi-x) are not bitwise identical
        EXPECT(types::is_approximately_equal(p.X, q.X));
        EXPECT(types::is_approximately_equal(p.Y, q.Y));
        EXPECT(types::is_approximately_equal(p.Z, q.Z));
    }
}


CASE("two-unit sphere") {
    struct DatumTwoUnits {
        static double radius() { return 2.; }
    };

    using geometry::UnitSphere;
    using TwoUnitsSphere = geometry::SphereT<DatumTwoUnits>;

    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai


    SECTION("radius") {
        EXPECT(TwoUnitsSphere::radius() == 2.);
    }


    SECTION("distances") {
        auto distance_1 = UnitSphere::distance(P1, P2);
        auto distance_2 = TwoUnitsSphere::distance(P1, P2);
        EXPECT(2. * distance_1 == distance_2);
    }


    SECTION("area globe") {
        auto area_1 = UnitSphere::area();
        auto area_2 = TwoUnitsSphere::area();
        EXPECT(4. * area_1 == area_2);
    }


    SECTION("sub areas") {
        area::BoundingBox bbox({P2.lat, P1.lon, P1.lat, P2.lon});

        auto area_1 = UnitSphere::area(bbox);
        auto area_2 = TwoUnitsSphere::area(bbox);
        EXPECT(4. * area_1 == area_2);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
