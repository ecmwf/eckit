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
#include <limits>

#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/SphereT.h"
#include "eckit/geo/UnitSphere.h"
#include "eckit/testing/Test.h"

namespace eckit::tests::geo {


using namespace eckit::geo;


// set sphere
struct DatumTwoUnits {
    static double radius() { return 2.; }
};

using TwoUnitsSphere = SphereT<DatumTwoUnits>;

const double R = UnitSphere::radius();


// -----------------------------------------------------------------------------
// test unit sphere radius

CASE("test unit sphere radius") {
    EXPECT(UnitSphere::radius() == 1.);
}

// -----------------------------------------------------------------------------
// test unit sphere poles

CASE("test unit sphere north pole") {
    auto p = UnitSphere::convertSphericalToCartesian({0., 90.});

    EXPECT(p.X == 0);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == R);
}

CASE("test unit sphere south pole") {
    auto p = UnitSphere::convertSphericalToCartesian({0., -90.});

    EXPECT(p.X == 0);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == -R);
}

// -----------------------------------------------------------------------------
// test unit sphere quadrants

CASE("test unit sphere lon 0") {
    auto p = UnitSphere::convertSphericalToCartesian({0., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-360., 0.});

    EXPECT(p.X == R);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 90") {
    auto p = UnitSphere::convertSphericalToCartesian({90., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-270., 0.});

    EXPECT(p.X == 0);
    EXPECT(p.Y == R);
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 180") {
    auto p = UnitSphere::convertSphericalToCartesian({180., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-180., 0.});

    EXPECT(p.X == -R);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 270") {
    auto p = UnitSphere::convertSphericalToCartesian({270., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-90., 0.});

    EXPECT(p.X == 0);
    EXPECT(p.Y == -R);
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}


// -----------------------------------------------------------------------------
// test unit sphere octants

const double L = R * std::sqrt(2) / 2.;

CASE("test unit sphere lon 45") {
    auto p = UnitSphere::convertSphericalToCartesian({45., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-315., 0.});

    EXPECT(types::is_approximately_equal(p.X, L));
    EXPECT(types::is_approximately_equal(p.Y, L));
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 135") {
    auto p = UnitSphere::convertSphericalToCartesian({135., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-225., 0.});

    EXPECT(types::is_approximately_equal(p.X, -L));
    EXPECT(types::is_approximately_equal(p.Y, L));
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 225") {
    auto p = UnitSphere::convertSphericalToCartesian({225., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-135., 0.});

    EXPECT(types::is_approximately_equal(p.X, -L));
    EXPECT(types::is_approximately_equal(p.Y, -L));
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

CASE("test unit sphere lon 315") {
    auto p = UnitSphere::convertSphericalToCartesian({315., 0.});
    auto q = UnitSphere::convertSphericalToCartesian({-45., 0.});

    EXPECT(types::is_approximately_equal(p.X, L));
    EXPECT(types::is_approximately_equal(p.Y, -L));
    EXPECT(p.Z == 0);

    EXPECT(Point3::equal(p, q));
}

// -----------------------------------------------------------------------------
// test unit sphere with non-canonical latitudes outside [-90, 90]

CASE("test unit sphere lat 100") {
    // Default behavior throws
    EXPECT_THROWS_AS(PointLonLat(0., 100.), BadValue);
    EXPECT_THROWS_AS(UnitSphere::convertSphericalToCartesian({0., 100.}), BadValue);

    auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(0., 100.), 0.);
    auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(180., 80.), 0.);

    // sin(x) and sin(pi-x) are not bitwise identical
    EXPECT(types::is_approximately_equal(p.X, q.X));
    EXPECT(types::is_approximately_equal(p.Y, q.Y));
    EXPECT(types::is_approximately_equal(p.Z, q.Z));
}

CASE("test unit sphere lat 290") {
    // Default behavior throws
    EXPECT_THROWS_AS(PointLonLat(15., 290.), BadValue);
    EXPECT_THROWS_AS(UnitSphere::convertSphericalToCartesian({15., 290.}), BadValue);

    auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(15., 290.), 0.);
    auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(15., -70.), 0.);

    // sin(x) and sin(pi-x) are not bitwise identical
    EXPECT(types::is_approximately_equal(p.X, q.X));
    EXPECT(types::is_approximately_equal(p.Y, q.Y));
    EXPECT(types::is_approximately_equal(p.Z, q.Z));
}

CASE("test unit sphere lat -120") {
    // Default behavior throws
    EXPECT_THROWS_AS(PointLonLat(45., -120.), BadValue);
    EXPECT_THROWS_AS(UnitSphere::convertSphericalToCartesian({45., -120.}), BadValue);

    auto p = UnitSphere::convertSphericalToCartesian(PointLonLat::make(45., -120.), 0.);
    auto q = UnitSphere::convertSphericalToCartesian(PointLonLat::make(225., -60.), 0.);

    // sin(x) and sin(pi-x) are not bitwise identical
    EXPECT(types::is_approximately_equal(p.X, q.X));
    EXPECT(types::is_approximately_equal(p.Y, q.Y));
    EXPECT(types::is_approximately_equal(p.Z, q.Z));
}

// -----------------------------------------------------------------------------
// test unit sphere distances with non-canonical coordinates

CASE("test unit sphere distances") {
    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai

    // Same points with added shifts
    const auto P1b = PointLonLat::make(288.4, -33.);    // Valparaíso + longitude shift
    const auto P2b = PointLonLat::make(301.8, 148.6);   // Shanghai + latitude/longitude shift
    const auto P2c = PointLonLat::make(-58.2, -211.4);  // Shanghai + latitude/longitude shift

    const double d0 = UnitSphere::distance(P1, P2);
    const double d1 = UnitSphere::distance(P1b, P2);
    const double d2 = UnitSphere::distance(P1, P2b);
    const double d3 = UnitSphere::distance(P1, P2c);

    EXPECT(types::is_approximately_equal(d0, d1));
    EXPECT(types::is_approximately_equal(d0, d2));
    EXPECT(types::is_approximately_equal(d0, d3));
}

// -----------------------------------------------------------------------------
// test unit sphere area

CASE("test unit sphere area globe") {
    EXPECT(UnitSphere::area() == 4. * M_PI * R * R);
}

CASE("test unit sphere area hemispheres") {
    const double area_hemisphere_north = UnitSphere::area({-180., 90.}, {180., 0.});
    const double area_hemisphere_south = UnitSphere::area({-180., 0.}, {180., -90.});

    EXPECT(area_hemisphere_north == 0.5 * UnitSphere::area());
    EXPECT(area_hemisphere_north == area_hemisphere_south);
}

// -----------------------------------------------------------------------------
// test two units sphere

CASE("test two units sphere radius") {
    EXPECT(TwoUnitsSphere::radius() == 2.);
}

CASE("test two units sphere distances") {
    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai

    const double d_sphere_1 = UnitSphere::distance(P1, P2);
    const double d_sphere_2 = TwoUnitsSphere::distance(P1, P2);
    EXPECT(2. * d_sphere_1 == d_sphere_2);
}

CASE("test two units sphere areas") {
    const double area_sphere_1 = UnitSphere::area();
    const double area_sphere_2 = TwoUnitsSphere::area();
    EXPECT(4. * area_sphere_1 == area_sphere_2);
}

CASE("test two units sphere sub areas") {
    const PointLonLat P1(-71.6, -33.);  // Valparaíso
    const PointLonLat P2(121.8, 31.4);  // Shanghai

    const double sub_area_sphere_1 = UnitSphere::area(P2, P1);
    const double sub_area_sphere_2 = TwoUnitsSphere::area(P2, P1);
    EXPECT(4. * sub_area_sphere_1 == sub_area_sphere_2);
}

// -----------------------------------------------------------------------------

}  // namespace eckit::tests::geo

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
