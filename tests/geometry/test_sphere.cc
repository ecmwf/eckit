/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <limits>

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/geometry/UnitSphere.h"
#include "eckit/geometry/SphereT.h"
#include "eckit/testing/Test.h"


using namespace eckit::testing;

namespace eckit {
namespace test {

using namespace geometry;

struct PointLonLat : Point2 {
    PointLonLat( double x, double y ) : Point2(x, y) {}
    const double& lon() const { return x_[0]; }
    const double& lat() const { return x_[1]; }
};

struct PointXYZ : Point3 {
    const double& x() const { return x_[0]; }
    const double& y() const { return x_[1]; }
    const double& z() const { return x_[2]; }
};

// set sphere
struct DatumTwoUnits {
    static double radius() {
        return 2.;
    }
};

typedef SphereT<DatumTwoUnits> TwoUnitsSphere;

const double R = UnitSphere::radius();


// -----------------------------------------------------------------------------
// test_unit_sphere_radius

CASE( "test_unit_sphere_radius" )
{
    EXPECT(UnitSphere::radius() == 1.);
}

// -----------------------------------------------------------------------------
// test_unit_sphere_poles

CASE( "test_unit_sphere_north_pole" )
{
    const PointLonLat ll1(0., 90.);
    PointXYZ p;
    UnitSphere::convertSphericalToCartesian(ll1, p);

    EXPECT(p.x() == 0);
    EXPECT(p.y() == 0);
    EXPECT(p.z() == R);
}

CASE( "test_unit_sphere_south_pole" )
{
    const PointLonLat ll1(0., -90.);
    PointXYZ p;
    UnitSphere::convertSphericalToCartesian(ll1, p);

    EXPECT(p.x() == 0);
    EXPECT(p.y() == 0);
    EXPECT(p.z() == -R);
}

// -----------------------------------------------------------------------------
// test_unit_sphere_quadrants

CASE( "test_unit_sphere_lon_0" )
{
    const PointLonLat ll1(0., 0.);
    const PointLonLat ll2(-360., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT(p.x() == R);
    EXPECT(p.y() == 0);
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_90" )
{
    const PointLonLat ll1(90., 0.);
    const PointLonLat ll2(-270., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT(p.x() == 0);
    EXPECT(p.y() == R);
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_180" )
{
    const PointLonLat ll1(180., 0.);
    const PointLonLat ll2(-180., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT(p.x() == -R);
    EXPECT(p.y() == 0);
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_270" )
{
    const PointLonLat ll1(270., 0.);
    const PointLonLat ll2(-90., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT(p.x() == 0);
    EXPECT(p.y() == -R);
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}


// -----------------------------------------------------------------------------
// test_unit_sphere_octants

const double L = R * std::sqrt(2) / 2.;

CASE( "test_unit_sphere_lon_45" )
{
    const PointLonLat ll1(45., 0.);
    const PointLonLat ll2(-315., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT( eckit::types::is_approximately_equal( p.x(), L) );
    EXPECT( eckit::types::is_approximately_equal( p.y(), L) );
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_135" )
{
    const PointLonLat ll1(135., 0.);
    const PointLonLat ll2(-225., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT( eckit::types::is_approximately_equal( p.x(), -L) );
    EXPECT( eckit::types::is_approximately_equal( p.y(), L) );
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_225" )
{
    const PointLonLat ll1(225., 0.);
    const PointLonLat ll2(-135., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT( eckit::types::is_approximately_equal( p.x(), -L) );
    EXPECT( eckit::types::is_approximately_equal( p.y(), -L) );
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_lon_315" )
{
    const PointLonLat ll1(315., 0.);
    const PointLonLat ll2(-45., 0.);
    PointXYZ p, q;
    UnitSphere::convertSphericalToCartesian(ll1, p);
    UnitSphere::convertSphericalToCartesian(ll2, q);

    EXPECT( eckit::types::is_approximately_equal( p.x(), L) );
    EXPECT( eckit::types::is_approximately_equal( p.y(), -L) );
    EXPECT(p.z() == 0);

    EXPECT(PointXYZ::equal(p, q));
}

CASE( "test_unit_sphere_great_circle_latitude_given_longitude" )
{
    // latitude at Valparaíso-Shanghai mid-point
    const PointLonLat P1(-71.6, -33.);
    const PointLonLat P2(121.8,  31.4);

    double lon = -159.18;
    double lat = UnitSphere::greatCircleLatitudeGivenLongitude(P1, P2, lon);

    EXPECT( eckit::types::is_approximately_equal( lat, -6.81, 0.01 ) );
}

// -----------------------------------------------------------------------------
// test_unit_sphere_area

CASE( "test_unit_sphere_area_globe" )
{
    EXPECT(UnitSphere::area() == 4. * M_PI * R * R);
}

CASE( "test_unit_sphere_area_hemispheres" )
{
    const PointLonLat ll1(-180.,  90.);
    const PointLonLat ll2( 180.,   0.);
    const PointLonLat ll3(-180.,   0.);
    const PointLonLat ll4( 180., -90.);
    const double area_hemisphere_north = UnitSphere::area(ll1, ll2);
    const double area_hemisphere_south = UnitSphere::area(ll3, ll4);

    EXPECT(area_hemisphere_north == 0.5 * UnitSphere::area());
    EXPECT(area_hemisphere_north == area_hemisphere_south);
}

// -----------------------------------------------------------------------------
// test_two_units_sphere

CASE( "test_two_units_sphere_radius" )
{
    EXPECT(TwoUnitsSphere::radius() == 2.);
}

CASE( "test_two_units_sphere_distances" )
{
    const PointLonLat P1(-71.6, -33.);   // Valparaíso
    const PointLonLat P2(121.8,  31.4);  // Shanghai

    const double d_sphere_1 = UnitSphere::distance(P1, P2);
    const double d_sphere_2 = TwoUnitsSphere::distance(P1, P2);
    EXPECT(2. * d_sphere_1 == d_sphere_2);
}

CASE( "test_two_units_sphere_areas" )
{
    const double area_sphere_1 = UnitSphere::area();
    const double area_sphere_2 = TwoUnitsSphere::area();
    EXPECT(4. * area_sphere_1 == area_sphere_2);
}

CASE( "test_two_units_sphere_sub_areas" )
{
    const PointLonLat P1(-71.6, -33.);   // Valparaíso
    const PointLonLat P2(121.8,  31.4);  // Shanghai

    const double sub_area_sphere_1 = UnitSphere::area(P2, P1);
    const double sub_area_sphere_2 = TwoUnitsSphere::area(P2, P1);
    EXPECT(4. * sub_area_sphere_1 == sub_area_sphere_2);
}

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv) {
    return run_tests(argc, argv);
}
