/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <limits>

#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"
#include "eckit/geometry/SphereT.h"
#include "eckit/geometry/UnitSphere.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

using namespace geometry;

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
    const PointLonLat ll1(0., 90.);
    auto p = UnitSphere::convertSphericalToCartesian(ll1);

    EXPECT(p.X == 0);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == R);
}

CASE("test unit sphere south pole") {
    const PointLonLat ll1(0., -90.);
    auto p = UnitSphere::convertSphericalToCartesian(ll1);

    EXPECT(p.X == 0);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == -R);
}

// -----------------------------------------------------------------------------
// test unit sphere quadrants

CASE("test unit sphere lon 0") {
    const PointLonLat ll1(0., 0.);
    const PointLonLat ll2(-360., 0.);
    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(p.X == R);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 90") {
    const PointLonLat ll1(90., 0.);
    const PointLonLat ll2(-270., 0.);
    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(p.X == 0);
    EXPECT(p.Y == R);
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 180") {
    const PointLonLat ll1(180., 0.);
    const PointLonLat ll2(-180., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(p.X == -R);
    EXPECT(p.Y == 0);
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 270") {
    const PointLonLat ll1(270., 0.);
    const PointLonLat ll2(-90., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(p.X == 0);
    EXPECT(p.Y == -R);
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}


// -----------------------------------------------------------------------------
// test unit sphere octants

const double L = R * std::sqrt(2) / 2.;

CASE("test unit sphere lon 45") {
    const PointLonLat ll1(45., 0.);
    const PointLonLat ll2(-315., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(eckit::types::is_approximately_equal(p.X, L));
    EXPECT(eckit::types::is_approximately_equal(p.Y, L));
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 135") {
    const PointLonLat ll1(135., 0.);
    const PointLonLat ll2(-225., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(eckit::types::is_approximately_equal(p.X, -L));
    EXPECT(eckit::types::is_approximately_equal(p.Y, L));
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 225") {
    const PointLonLat ll1(225., 0.);
    const PointLonLat ll2(-135., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(eckit::types::is_approximately_equal(p.X, -L));
    EXPECT(eckit::types::is_approximately_equal(p.Y, -L));
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

CASE("test unit sphere lon 315") {
    const PointLonLat ll1(315., 0.);
    const PointLonLat ll2(-45., 0.);

    auto p = UnitSphere::convertSphericalToCartesian(ll1);
    auto q = UnitSphere::convertSphericalToCartesian(ll2);

    EXPECT(eckit::types::is_approximately_equal(p.X, L));
    EXPECT(eckit::types::is_approximately_equal(p.Y, -L));
    EXPECT(p.Z == 0);

    EXPECT(points_equal(p, q));
}

// -----------------------------------------------------------------------------
// test unit sphere area

CASE("test unit sphere area globe") {
    EXPECT(UnitSphere::area() == 4. * M_PI * R * R);
}

CASE("test unit sphere area hemispheres") {
    const PointLonLat ll1(-180., 90.);
    const PointLonLat ll2(180., 0.);
    const PointLonLat ll3(-180., 0.);
    const PointLonLat ll4(180., -90.);
    const double area_hemisphere_north = UnitSphere::area(ll1, ll2);
    const double area_hemisphere_south = UnitSphere::area(ll3, ll4);

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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
