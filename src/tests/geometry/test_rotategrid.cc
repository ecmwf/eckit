/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <iostream>

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE TestRotatedLatLon
#include "ecbuild/boost_test_framework.h"

#include "eckit/geometry/RotateGrid.h"

using namespace std;
using namespace eckit;
using namespace eckit::geometry;

static void rotgrid_py(double sp_lat,double sp_lon, double sp_rot, double lat, double lon)
{
  // RotgridPy is used as reference to test RotateGrid, and magics rotate/unrotate
  //
  // Location of pole of rotated grid as seen in non-rotated grid: sp_lat,sp_lon ;
  // Additional axial rotation about pole of rotated grid: sp_rot
  // Location of chosen point in non-rotated grid  lat,lon
  // Location of chosen point as seen in rotated grid  tr_point
  // Location of chosen point put back into non-rotated grid tr_point2
   RotgridPy mapping(sp_lat,sp_lon, sp_rot);
   std::pair<double,double> tr_point = mapping.transform(lat,lon);
   std::pair<double,double> tr_point2 = mapping.transform(tr_point.first, tr_point.second, true);
   std::cout << " sp(" << sp_lat << "," << sp_lon << ")"
            << " sp_rot(" << sp_rot << ") " << "(" << lat << "," << lon << ")"
            << "    Py rotated " << "(" << tr_point.first << "," << tr_point.second << ")"
            << " unrotated  " << "(" << tr_point2.first << "," << tr_point2.second << ")"
            << "\n";
}

static double degrees_eps()
{
   /// default is 1E-3 because
   /// some bugs in IFS means we need a lower resolution epsilon when decoding from grib2
   return 1E-3 ;
}


BOOST_AUTO_TEST_SUITE( TestRotateGrid )


BOOST_AUTO_TEST_CASE( test_rotated_lat_lon )
{
   cout << "\nGrid:: ...test_rotated_lat_lon\n";

   // Compare RotateGrid, with RotgridPy, and magics rotate/unrotate/
   // Note: magics rotate/unrotate does not support southern pole rotation angle

   double sp_lat = 37.5;
   double sp_lon = 177.5;
   double polerot = 10.0 ;
   eckit::geometry::LLPoint2 south_pole(sp_lon,sp_lat);

   double lat = 51.0 ;
   double lon = -3.0 ;
   eckit::geometry::LLPoint2 point(lon,lat);

   rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);
   {
      RotateGrid mapping(south_pole , polerot);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}


BOOST_AUTO_TEST_CASE( test_rotated_lat_lon_2 )
{
   cout << "\nGrid:: ...test_rotated_lat_lon_2\n";

   // Compare RotateGrid, with RotgridPy, and magics rotate/unrotate/
   // Note: magics rotate/unrotate does not support southern pole rotation angle

   double sp_lat = 18;
   double sp_lon = -39;
   eckit::geometry::LLPoint2 south_pole(sp_lon,sp_lat);

   double polerot = 0.0 ;
   RotateGrid mapping(south_pole, polerot);
   {
      double lat = 12.0 ;
      double lon = 55.0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lat,lon);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }

   {
      double lat = 12.0 ;
      double lon = 54.0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lat);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }

   {
      double lat = 12.0 ;
      double lon = 53.0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lat);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout <<  " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}

BOOST_AUTO_TEST_CASE( test_south_pole_at_south_pole )
{
   cout << "\nGrid:: ...test_south_pole_at_south_pole\n";

   // Rotation of -90,0 for south pole, should mean no change, since -90,0 is at the south pole
   // Should end up with identity matrix and hence no change, likewise for un-rotate
   double sp_lat = -90.0;
   double sp_lon = 0;
   double polerot = 0.0 ;
   eckit::geometry::LLPoint2 south_pole(sp_lon,sp_lat);
   RotateGrid mapping(south_pole, polerot);
   {
      double lat = 12.0 ;
      double lon = 55.0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lat);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),rotated1.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),rotated1.lon(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}

BOOST_AUTO_TEST_CASE( test_south_pole_at_north_pole )
{
   cout << "\nGrid:: ...test_south_pole_at_north_pole\n";

   // Change south pole to north pole ?
   double sp_lat = 90.0;
   double sp_lon = 0;
   double polerot = 0.0 ;
   eckit::geometry::LLPoint2 south_pole(sp_lon,sp_lat);
   RotateGrid mapping(south_pole, polerot);
   {
      double lat = 12.0 ;
      double lon = 55.0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lat);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(-12.0,rotated1.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(125.0,rotated1.lon(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}

BOOST_AUTO_TEST_CASE( test_south_pole_at_equator )
{
   cout << "\nGrid:: ...test_south_pole_at_equator\n";

   // Move south pole to the equator
   // Henve a poit at the south pole ,  shold move to the equator
   double sp_lat = 0;
   double sp_lon = 0;
   double polerot = 0.0 ;
   eckit::geometry::LLPoint2 south_pole(sp_lat,sp_lon);
   RotateGrid mapping(south_pole, polerot);
   {
      double lat = 0 ;
      double lon = 0 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lon);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(rotated1.lat(),-90.0,degrees_eps());
      BOOST_CHECK_CLOSE(rotated1.lon(),  0.0,degrees_eps());
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}

BOOST_AUTO_TEST_CASE( test_south_pole_at_minus_90_0_rot_10 )
{
   cout << "\nGrid:: ...test_south_pole_at_minus_90_0_rot_10\n";

   // South pole at -90,0 (i.e unchanged) but with a rotation angle of 10
   double sp_lat = -90;
   double sp_lon = 0;
   double polerot = 10.0 ;
   eckit::geometry::LLPoint2 south_pole(sp_lat,sp_lon);
   RotateGrid mapping(south_pole, polerot);
   {
      double lat = 12 ;
      double lon = 55 ;
      rotgrid_py(sp_lat,sp_lon,polerot,lat,lon);

      eckit::geometry::LLPoint2 point(lon,lat);
      eckit::geometry::LLPoint2 rotated = mapping.magics_rotate(point);
      eckit::geometry::LLPoint2 unrotated = mapping.magics_rotate(rotated);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " Magic rotated " <<  rotated << " unrotated " << unrotated << "  **ignores south pol rotation**\n";

      eckit::geometry::LLPoint2 rotated1 = mapping.rotate(point);
      eckit::geometry::LLPoint2 unrotated2 = mapping.unrotate(rotated1);
      std::cout << " sp" << south_pole << " sp_rot(" << polerot << ") " << point << " My    rotated " <<  rotated1 << " unrotated " << unrotated2 << "\n";
      BOOST_CHECK_CLOSE(point.lat(),unrotated2.lat(),degrees_eps());
      BOOST_CHECK_CLOSE(point.lon(),unrotated2.lon(),degrees_eps());
   }
}

BOOST_AUTO_TEST_SUITE_END()
