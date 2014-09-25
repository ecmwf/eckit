#include "eckit/geometry/RotateGrid.h"

namespace eckit {
namespace geometry {

//-----------------------------------------------------------------------------

RotateGrid::RotateGrid(const eckit::geometry::LLPoint2& south_pole,
                 double  south_pole_rot_angle,
                 double lonMin)
 : degree_to_radian_(M_PI / 180.0),
   radian_to_degree_(1/degree_to_radian_),
   south_pole_(south_pole),
   south_pole_rot_angle_(south_pole_rot_angle),
   lonmin_(lonMin),
   lonmax_(lonMin + 360.0)
{
}

// For reference, this what magics uses, it appears as if it originated from fortran code
// Unfortunately there is no reference. Tests show unrotate is broken
eckit::geometry::LLPoint2 RotateGrid::magics_rotate( const eckit::geometry::LLPoint2& point ) const
{
   double lat_y = point.lat();
   double lon_x = point.lon();

   double sin_south_pole_lat = sin(degree_to_radian_*(south_pole_.lat()+90.));
   double cos_south_pole_lat = cos(degree_to_radian_*(south_pole_.lat()+90.));

   double ZXMXC = degree_to_radian_*(lon_x - south_pole_.lon());
   double sin_lon_decr_sp = sin(ZXMXC);
   double cos_lon_decr_sp = cos(ZXMXC);
   double sin_lat = sin(degree_to_radian_*lat_y);
   double cos_lat = cos(degree_to_radian_*lat_y);
   double ZSYROT = cos_south_pole_lat*sin_lat - sin_south_pole_lat*cos_lat*cos_lon_decr_sp;
   ZSYROT = std::max( std::min(ZSYROT, +1.0), -1.0 );

   double PYROT = asin(ZSYROT)*radian_to_degree_;

   double ZCYROT = cos(PYROT*degree_to_radian_);
   double ZCXROT = (cos_south_pole_lat*cos_lat*cos_lon_decr_sp + sin_south_pole_lat*sin_lat)/ZCYROT;
   ZCXROT = std::max( std::min(ZCXROT, +1.0), -1.0 );
   double ZSXROT = cos_lat*sin_lon_decr_sp/ZCYROT;

   double PXROT = acos(ZCXROT)*radian_to_degree_;

   if( ZSXROT < 0.0)
      PXROT = -PXROT;

   return eckit::geometry::LLPoint2( PYROT, PXROT);
}

eckit::geometry::LLPoint2 RotateGrid::magics_unrotate( const eckit::geometry::LLPoint2& point ) const
{
   double lat_y = point.lat();
   double lon_x = point.lon();

   double sin_south_pole_lat = sin(degree_to_radian_*(south_pole_.lat()+90.));
   double cos_south_pole_lat = cos(degree_to_radian_*(south_pole_.lat()+90.));
   double sin_lon = sin(degree_to_radian_*lon_x);
   double cos_lon = cos(degree_to_radian_*lon_x);
   double sin_lat = sin(degree_to_radian_*lat_y);
   double cos_lat = cos(degree_to_radian_*lat_y);
   double ZSYREG = cos_south_pole_lat*sin_lat + sin_south_pole_lat*cos_lat*cos_lon;
   ZSYREG = std::max( std::min(ZSYREG, +1.0), -1.0 );
   double PYREG = asin(ZSYREG)*radian_to_degree_;
   double ZCYREG = cos(PYREG*degree_to_radian_);
   double ZCXMXC = (cos_south_pole_lat*cos_lat*cos_lon - sin_south_pole_lat*sin_lat)/ZCYREG;
   ZCXMXC = std::max( std::min(ZCXMXC, +1.0), -1.0 );
   double ZSXMXC = cos_lat*sin_lat/ZCYREG;
   double ZXMXC = acos(ZCXMXC)*radian_to_degree_;
   if( ZSXMXC < 0.0)
      ZXMXC = -ZXMXC;
   double PXREG = ZXMXC + south_pole_.lon();

   return eckit::geometry::LLPoint2( PYREG, PXREG);
}

eckit::geometry::LLPoint2 RotateGrid::rotate( const eckit::geometry::LLPoint2& point) const
{
   // See: http://rbrundritt.wordpress.com/2008/10/14/conversion-between-spherical-and-cartesian-coordinates-systems/
   // First convert the data point from spherical lat lon to (x',y',z') using:
   double latr = point.lat() * degree_to_radian_ ;
   double lonr = point.lon() * degree_to_radian_ ;
   double xd = cos(lonr)*cos(latr);
   double yd = sin(lonr)*cos(latr);
   double zd = sin(latr);

   // Assume right hand rule, rotate about z axes and then y
   // P' = Rot(y) * Rot(z) * Pv
   // x   (  cos(ϑ), 0, -sin(ϑ)) ( cos(φ), -sin(φ), 0) (x')
   // y = (  0     , 1,  0     ) ( sin(φ), cos(φ),  0) (y')
   // z   ( sin(ϑ), 0,   cos(ϑ)) ( 0     , 0     ,  1) (z')

   // Expanded
   // x   ( cos(ϑ)cos(φ) , -cos(ϑ)sin(φ) , -sin(ϑ)) (x')
   // y = ( sin(φ)       ,  cos(φ)       ,  0     ).(y')
   // z   ( sin(ϑ) cos(φ), -sin(ϑ) sin(φ),  cos(ϑ)) (z')

   double t = -(90.0 + south_pole_.lat());
   double o = -south_pole_.lon() + south_pole_rot_angle_;

   double sin_t = sin(degree_to_radian_ * t);
   double cos_t = cos(degree_to_radian_ * t);
   double sin_o = sin(degree_to_radian_ * o);
   double cos_o = cos(degree_to_radian_ * o);

   double x = cos_t*cos_o*xd - cos_t*sin_o*yd - sin_t*zd;
   double y = sin_o*xd + cos_o*yd  ;
   double z = sin_t*cos_o*xd - sin_t*sin_o*yd + cos_t*zd;

   // Then convert back to 'normal' (lat,lon)
   // z = r.cosϑ  ( r is earths radius, assume 1)
   // r = sqrt(x.x + y.y + z.z)
   // z = r.cosϑ  => ϑ = cos-1(z/r)
   // ϑ = con-1( z/ sqrt(x.x + y.y + z.z) )
   // By rearranging the formulas for the x and y components we can solve the value for the Φ angle.
   //    y                  x
   //   ---     = sinφ =   ----
   //   r.sinϑ             r.cosϑ
   //
   //  y/x = sinϑ/cosϑ = tanϑ
   //
   //  ϑ = tan-1(y/x)   => lon = atan2(y, x)


   // Uses arc sin, to convert back to degrees, put in range -1 to 1 in case of slight rounding error
   // avoid error on calculating e.g. asin(1.00000001)
   if (z > 1.0)  z = 1.0;
   if (z < -1.0) z = -1.0;
   double ret_lat = asin(z) * radian_to_degree_;
   double ret_lon = atan2(y, x) * radian_to_degree_;

   // Still get a very small rounding error, round to 6 decimal places
   ret_lat = roundf( ret_lat * 1000000.0 )/1000000.0;
   ret_lon = roundf( ret_lon * 1000000.0 )/1000000.0;

   // Make sure ret_lon is in range
   while (ret_lon < lonmin_) ret_lon += 360.0;
   while (ret_lon >= lonmax_) ret_lon -= 360.0;

   return eckit::geometry::LLPoint2(ret_lat,ret_lon);
}

eckit::geometry::LLPoint2 RotateGrid::unrotate( const eckit::geometry::LLPoint2& point) const
{
   // See: http://gis.stackexchange.com/questions/10808/lon-lat-transformation/14445
   // First convert the data point from spherical lat lon to (x',y',z') using:
   double latr = point.lat() * degree_to_radian_ ;
   double lonr = point.lon() * degree_to_radian_ ;
   double xd = cos(lonr)*cos(latr);
   double yd = sin(lonr)*cos(latr);
   double zd = sin(latr);

   // P' = Rot(z) * Rot(y) * Pv,   rotate about y axes then Z
   // Since we're undoing the rotation described in the definition of the coordinate system,
   // we first rotate by ϑ = -(90 + south_pole_lat) around the y' axis (along the rotated Greenwich meridian)
   // and then by φ = -south_pole_lon = +15 degrees around the z axis):
   // x   ( cos(φ), sin(φ), 0) (  cos(ϑ), 0, sin(ϑ)) (x')
   // y = (-sin(φ), cos(φ), 0).(  0     , 1, 0     ).(y')
   // z   ( 0     , 0     , 1) ( -sin(ϑ), 0, cos(ϑ)) (z')

   // Expanded
   // x =  cos(ϑ) cos(φ) x' + sin(φ) y' + sin(ϑ) cos(φ) z'
   // y = -cos(ϑ) sin(φ) x' + cos(φ) y' - sin(ϑ) sin(φ) z'
   // z = -sin(ϑ) x' + cos(ϑ) z'

   double t = -(90.0 + south_pole_.lat());
   double o = -south_pole_.lon();

   double sin_t = sin(degree_to_radian_ * t);
   double cos_t = cos(degree_to_radian_ * t);
   double sin_o = sin(degree_to_radian_ * o);
   double cos_o = cos(degree_to_radian_ * o);

   double x = cos_t*cos_o*xd + sin_o*yd + sin_t*cos_o*zd;
   double y = -cos_t*sin_o*xd + cos_o*yd - sin_t*sin_o*zd;
   double z = -sin_t*xd + cos_t*zd;

   // Then convert back to 'normal' (lat,lon) using
   // Uses arc sin, to convert back to degrees, put in range -1 to 1 in case of slight rounding error
   // avoid error on calculating e.g. asin(1.00000001)
   if (z > 1.0)  z = 1.0;
   if (z < -1.0) z = -1.0;

   double ret_lat = asin(z)*radian_to_degree_;
   double ret_lon = atan2(y, x)*radian_to_degree_;

   // Still get a very small rounding error, round to 6 decimal places
   ret_lat = roundf( ret_lat * 1000000.0 )/1000000.0;
   ret_lon = roundf( ret_lon * 1000000.0 )/1000000.0;

   ret_lon -= south_pole_rot_angle_;

   // Make sure ret_lon is in range
   while (ret_lon < lonmin_) ret_lon += 360.0;
   while (ret_lon >= lonmax_) ret_lon -= 360.0;

   return eckit::geometry::LLPoint2(ret_lat ,ret_lon);
}

//-----------------------------------------------------------------------------

// *** This is only used for test comparison *****
RotgridPy::RotgridPy(double south_pole_lat, double south_pole_lon,
                 double  south_pole_rot_angle,
                 double  nPoleGridLon,
                 double lonMin)
 : degree_to_radian_(M_PI / 180.0),
   radian_to_degree_(1/degree_to_radian_),
   south_pole_lat_(south_pole_lat),
   south_pole_lon_(south_pole_lon),
   south_pole_rot_angle_(south_pole_rot_angle),
   lonmin_(lonMin),
   lonmax_(lonMin + 360.0)
{
   double south_pole_lat_in_radians = south_pole_lat * degree_to_radian_ ;
   cossouth_pole_lat_ = cos(south_pole_lat_in_radians);
   sinsouth_pole_lat_ = sin(south_pole_lat_in_radians);

   if (nPoleGridLon != 0)
      south_pole_rot_angle_ = south_pole_lon - nPoleGridLon + 180.0;
}

std::pair<double,double> RotgridPy::transform(double lat, double lon, bool inverse) const
{
   // calculate trig terms relating to longitude
   double south_pole_lon = south_pole_lon_;

   if (inverse) {
      south_pole_lon += 180.0;
      lon += south_pole_rot_angle_;
   }

   double lon_decr_south_pole_lon = lon - south_pole_lon;
   double dlonr = lon_decr_south_pole_lon * degree_to_radian_;
   double cosdlonr = cos(dlonr);
   double sindlonr = sin(dlonr);

   // likewise for latitude
   double latr = lat * degree_to_radian_;
   double coslatr = cos(latr);
   double sinlatr = sin(latr);

   // now the main calculation
   std::pair<double,double> ret = rotgrid_core( cossouth_pole_lat_, sinsouth_pole_lat_,
                          cosdlonr, sindlonr,
                          coslatr, sinlatr );

   // first is long, second is lat
   double latrotr = ret.first;
   double dlonrotr = ret.second;

   double lonrot = south_pole_lon + dlonrotr * radian_to_degree_;
   double latrot = latrotr * radian_to_degree_;

   // Still get a very small rounding error, round to 6 decimal places
   lonrot = roundf( lonrot * 1000000.0 )/1000000.0;
   latrot = roundf( latrot * 1000000.0 )/1000000.0;

   if (!inverse)
      lonrot -= south_pole_rot_angle_;

   // put lonrot back in range
   while (lonrot < lonmin_) lonrot += 360.0;
   while (lonrot >= lonmax_) lonrot -= 360.0;

//   std::cout << "Transform returning (" << lonrot << "," <<  latrot << ")\n";
   return std::pair<double,double>(latrot,lonrot);
}

std::pair<double,double> RotgridPy::rotgrid_core(
               double cossouth_pole_lat, double sinsouth_pole_lat,
               double cosdlon, double sindlon, double coslat, double sinlat) const
{
    double cycdx = coslat * cosdlon ;

    // Evaluate rotated longitude, use atan2 to convert back to degrees
    double dlonrot = atan2(coslat * sindlon,
                             cycdx * sinsouth_pole_lat - sinlat * cossouth_pole_lat);

    // Evaluate rotated latitude
    double sinlatrot = cycdx * cossouth_pole_lat + sinlat * sinsouth_pole_lat ;

    // Uses arc sin, to convert back to degrees
    // put in range -1 to 1 in case of slight rounding error
    //  avoid error on calculating e.g. asin(1.00000001)
    if (sinlatrot > 1.0)  sinlatrot = 1.0;
    if (sinlatrot < -1.0)  sinlatrot = -1.0;

    double latrot = asin(sinlatrot);

    return std::pair<double,double>(latrot,dlonrot);
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

