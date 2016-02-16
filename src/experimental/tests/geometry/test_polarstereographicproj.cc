/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE test_eckit_geometry

#include "ecbuild/boost_test_framework.h"

#include "eckit/geometry/PolarStereoGraphicProj.h"

using namespace std;
using namespace eckit;
using namespace eckit::geometry;

//-----------------------------------------------------------------------------

namespace eckit_test {

static double degrees_eps()
{
   /// default is 1E-3 because
   /// some bugs in IFS means we need a lower resolution epsilon when decoding from grib2
   return 1E-3 ;
}

}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( TestPolarStereoGraphicProj )

BOOST_AUTO_TEST_CASE( test_north_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_north_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(false/*southPoleOnProjectionPlane*/,false/*earth_is_oblate*/,0 );
   ps.set_radius(6371229);

   double lat = 30;
   double lon = 30;
   eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lat) );
   eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[0],  pt_on_plane[1] );

   //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
   BOOST_CHECK_CLOSE(pt_on_sphere.lat(),lat,degrees_eps());
   BOOST_CHECK_CLOSE(pt_on_sphere.lon(),lon,degrees_eps());
}

BOOST_AUTO_TEST_CASE( test_south_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_south_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(true/*southPoleOnProjectionPlane*/,false/*earth_is_oblate*/,0);

   double lat = 30;
   double lon = 30;
   eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lat) );
   eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

   //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
   BOOST_CHECK_CLOSE(pt_on_sphere.lat(),lat,degrees_eps());
   BOOST_CHECK_CLOSE(pt_on_sphere.lon(),lon,degrees_eps());
}

BOOST_AUTO_TEST_CASE( test_global_north_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_global_north_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(false/*southPoleOnProjectionPlane*/,false/*earth_is_oblate*/,0);

   for(long lat = -90; lat < 90; lat++) {
      for(long lon = 0; lon < 360; lon++) {

         eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lat) );
         eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

         //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
         BOOST_CHECK_CLOSE(pt_on_sphere.lat(),(double)lat,degrees_eps());
         BOOST_CHECK_CLOSE(pt_on_sphere.lon(),(double)lon,degrees_eps());
      }
   }
}

BOOST_AUTO_TEST_CASE( test_global_south_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_global_south_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(true/*southPoleOnProjectionPlane*/,false/*earth_is_oblate*/,0);

   for(long lat = -90; lat < 90; lat++) {
      for(long lon = 0; lon < 360; lon++) {

         eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lon) );
         eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

         //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
         BOOST_CHECK_CLOSE(pt_on_sphere.lat(),(double)lat,degrees_eps());
         BOOST_CHECK_CLOSE(pt_on_sphere.lon(),(double)lon,degrees_eps());
      }
   }
}

BOOST_AUTO_TEST_CASE( test_north_ellipsoid_polar_stereographic_projection )
{
   cout << "\nGeometry:: ...test_north_ellipsoid_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(false/*southPoleOnProjectionPlane*/,true/*earth_is_oblate*/,0);
   ps.set_false_easting( 2000000);
   ps.set_false_northing(2000000);

   double lat = 73;
   double lon = 44;
   eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lat) );
   eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

   // cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
   BOOST_CHECK_CLOSE(pt_on_plane[0],3320416.75,degrees_eps());
   BOOST_CHECK_CLOSE(pt_on_plane[1],632668.43,degrees_eps());

   BOOST_CHECK_CLOSE(pt_on_sphere.lat(),lat,degrees_eps());
   BOOST_CHECK_CLOSE(pt_on_sphere.lon(),lon,degrees_eps());
}


BOOST_AUTO_TEST_CASE( test_global_north_ellipsoid_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_global_north_ellipsoid_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(false/*southPoleOnProjectionPlane*/,true/*earth_is_oblate*/,0);

   for(long lat = -90; lat < 90; lat++) {
      for(long lon = 0; lon < 360; lon++) {

         eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lat,lon) );
         eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

         //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
         BOOST_CHECK_CLOSE(pt_on_sphere.lat(),(double)lat,degrees_eps());
         BOOST_CHECK_CLOSE(pt_on_sphere.lon(),(double)lon,degrees_eps());
      }
   }
}

BOOST_AUTO_TEST_CASE( test_global_south_ellipsoid_polar_stereographic_projection )
{
   cout << "Geometry:: ...test_global_south_ellipsoid_polar_stereographic_projection\n";

   PolarStereoGraphicProj ps(true/*southPoleOnProjectionPlane*/,true/*earth_is_oblate*/,0);

   for(long lat = -90; lat < 90; lat++) {
      for(long lon = 0; lon < 360; lon++) {

         eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lon,lat) );
         eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[LON],  pt_on_plane[LAT] );

         //cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
         BOOST_CHECK_CLOSE(pt_on_sphere.lat(),(double)lat,degrees_eps());
         BOOST_CHECK_CLOSE(pt_on_sphere.lon(),(double)lon,degrees_eps());
      }
   }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
