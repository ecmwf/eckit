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
    using Point2::Point2;
    const double& lon() const { return x_[0]; }
    const double& lat() const { return x_[1]; }
};

struct PointXYZ : Point3 {
    using Point3::Point3;
    const double& x() const { return x_[0]; }
    const double& y() const { return x_[1]; }
    const double& z() const { return x_[2]; }
};

// set sphere
struct DatumTwoUnits {
    static double radiusInMeters() {
        return 2.;
    }
};

typedef SphereT<DatumTwoUnits> TwoUnitsSphere;

const double R = UnitSphere::radiusInMeters();


// -----------------------------------------------------------------------------
// test_unit_sphere_radius

CASE( "test_unit_sphere_radius" )
{
    EXPECT(UnitSphere::radiusInMeters() == 1.);
}

// -----------------------------------------------------------------------------
// test_unit_sphere_poles

CASE( "test_unit_sphere_north_pole" )
{
    const PointLonLat p1(0., 90.);
    PointXYZ p2;
    UnitSphere::convertSphericalToCartesian(p1, p2);

    EXPECT(p2.x() == 0);
    EXPECT(p2.y() == 0);
    EXPECT(p2.z() == R);
}

CASE( "test_unit_sphere_south_pole" )
{
    const PointLonLat p1(0., -90.);
    PointXYZ p2;
    UnitSphere::convertSphericalToCartesian(p1, p2);

    EXPECT(p2.x() == 0);
    EXPECT(p2.y() == 0);
    EXPECT(p2.z() == -R);
}

// -----------------------------------------------------------------------------
// test_unit_sphere_quadrants

CASE( "test_unit_sphere_lon_0" )
{
    const PointLonLat p1[2] = {{0., 0.}, {-360., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT(p2[0].x() == R);
    EXPECT(p2[0].y() == 0);
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_90" )
{
    const PointLonLat p1[2] = {{90., 0.}, {-270., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT(p2[0].x() == 0);
    EXPECT(p2[0].y() == R);
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_180" )
{
    const PointLonLat p1[2] = {{180., 0.}, {-180., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT(p2[0].x() == -R);
    EXPECT(p2[0].y() == 0);
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_270" )
{
    const PointLonLat p1[2] = {{270., 0.}, {-90., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT(p2[0].x() == 0);
    EXPECT(p2[0].y() == -R);
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}


// -----------------------------------------------------------------------------
// test_unit_sphere_octants

const double L = R * std::sqrt(2) / 2.;

CASE( "test_unit_sphere_lon_45" )
{
    const PointLonLat p1[2] = {{45., 0.}, {-315., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT( eckit::types::is_approximately_equal( p2[0].x(), L) );
    EXPECT( eckit::types::is_approximately_equal( p2[0].y(), L) );
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_135" )
{
    const PointLonLat p1[2] = {{135., 0.}, {-225., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT( eckit::types::is_approximately_equal( p2[0].x(), -L) );
    EXPECT( eckit::types::is_approximately_equal( p2[0].y(), L) );
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_225" )
{
    const PointLonLat p1[2] = {{225., 0.}, {-135., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT( eckit::types::is_approximately_equal( p2[0].x(), -L) );
    EXPECT( eckit::types::is_approximately_equal( p2[0].y(), -L) );
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_lon_315" )
{
    const PointLonLat p1[2] = {{315., 0.}, {-45., 0.}};
    PointXYZ p2[2];
    UnitSphere::convertSphericalToCartesian(p1[0], p2[0]);
    UnitSphere::convertSphericalToCartesian(p1[1], p2[1]);

    EXPECT( eckit::types::is_approximately_equal( p2[0].x(), L) );
    EXPECT( eckit::types::is_approximately_equal( p2[0].y(), -L) );
    EXPECT(p2[0].z() == 0);

    EXPECT(PointXYZ::equal(p2[0], p2[1]));
}

CASE( "test_unit_sphere_great_circle_latitude_given_longitude" )
{
    // latitude at Valparaíso-Shanghai mid-point
    const PointLonLat P1(-71.6, -33.);
    const PointLonLat P2(121.8,  31.4);

    PointLonLat midpoint(-159.18, std::numeric_limits<double>::quiet_NaN());
    UnitSphere::greatCircleLatitudeGivenLongitude(P1, P2, midpoint);

    EXPECT( eckit::types::is_approximately_equal( midpoint.lat(), -6.81, 0.01 ) );
}

// -----------------------------------------------------------------------------
// test_unit_sphere_area

CASE( "test_unit_sphere_area_globe" )
{
    EXPECT(UnitSphere::areaInSqMeters() == 4. * M_PI * R * R);
}

CASE( "test_unit_sphere_area_hemispheres" )
{
    const PointLonLat p[4] = {
        {-180., 90.}, {180.,   0.},
        {-180.,  0.}, {180., -90.}
    };
    const double area_hemisphere_north = UnitSphere::areaInSqMeters(p[0], p[1]);
    const double area_hemisphere_south = UnitSphere::areaInSqMeters(p[2], p[3]);

    EXPECT(area_hemisphere_north == 0.5 * UnitSphere::areaInSqMeters());
    EXPECT(area_hemisphere_north == area_hemisphere_south);
}

// -----------------------------------------------------------------------------
// test_two_units_sphere

CASE( "test_two_units_sphere_radius" )
{
    EXPECT(TwoUnitsSphere::radiusInMeters() == 2.);
}

CASE( "test_two_units_sphere_distances" )
{
    const PointLonLat P1(-71.6, -33.);   // Valparaíso
    const PointLonLat P2(121.8,  31.4);  // Shanghai

    const double d_sphere_1 = UnitSphere::distanceInMeters(P1, P2);
    const double d_sphere_2 = TwoUnitsSphere::distanceInMeters(P1, P2);
    EXPECT(2. * d_sphere_1 == d_sphere_2);
}

CASE( "test_two_units_sphere_areas" )
{
    const double area_sphere_1 = UnitSphere::areaInSqMeters();
    const double area_sphere_2 = TwoUnitsSphere::areaInSqMeters();
    EXPECT(4. * area_sphere_1 == area_sphere_2);
}

CASE( "test_two_units_sphere_sub_areas" )
{
    const PointLonLat P1(-71.6, -33.);   // Valparaíso
    const PointLonLat P2(121.8,  31.4);  // Shanghai

    const double sub_area_sphere_1 = UnitSphere::areaInSqMeters(P2, P1);
    const double sub_area_sphere_2 = TwoUnitsSphere::areaInSqMeters(P2, P1);
    EXPECT(4. * sub_area_sphere_1 == sub_area_sphere_2);
}

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv) {
    return run_tests(argc, argv);
}
