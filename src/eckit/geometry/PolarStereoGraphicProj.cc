#include "eckit/geometry/PolarStereoGraphicProj.h"

namespace eckit {
namespace geometry {

//-----------------------------------------------------------------------------
// See: http://mathworld.wolfram.com/StereographicProjection.html
//

PolarStereoGraphicProj::PolarStereoGraphicProj(
         bool southPoleOnProjectionPlane,
         bool earth_is_oblate,
         long lon_of_natural_origin
)
: proj_(NULL)
{
   if (earth_is_oblate) {
      proj_.reset(new EllipPolarStereoGraphicProj(southPoleOnProjectionPlane,lon_of_natural_origin));
   }
   else {
      proj_.reset(new SphericalPolarStereoGraphicProj(southPoleOnProjectionPlane,lon_of_natural_origin));
   }
}

PolarStereoGraphicProj::~PolarStereoGraphicProj(){}

eckit::geometry::Point2 PolarStereoGraphicProj::map_to_plane( const eckit::geometry::LLPoint2& p ) const
{
   return proj_->map_to_plane(p);
}

eckit::geometry::LLPoint2 PolarStereoGraphicProj::map_to_spherical( double x, double y ) const
{
   return proj_->map_to_spherical(x, y);
}

void PolarStereoGraphicProj::set_radius( double r )
{
   proj_->set_radius(r);
}

void PolarStereoGraphicProj::set_scale_factor_at_natural_origin( double Ko )
{
   proj_->set_scale_factor_at_natural_origin(Ko);
}

void PolarStereoGraphicProj::set_eccentricity( double e )
{
   proj_->set_eccentricity(e);
}

void PolarStereoGraphicProj::set_false_easting( double fe )
{
   proj_->set_false_easting(fe);
}

void PolarStereoGraphicProj::set_false_northing( double fn )
{
   proj_->set_false_northing(fn);
}


// ------------------------------------------------------------------------

AbsPolarStereoGraphicProj::AbsPolarStereoGraphicProj(
         bool southPoleOnProjectionPlane,
         long lon_of_natural_origin                 /* Lov in grib talk */
)
: degree_to_radian_(M_PI / 180.0),
  radian_to_degree_(1/degree_to_radian_),
  radius_(1),
  southPoleOnProjectionPlane_(southPoleOnProjectionPlane),
  lon_of_natural_origin_(lon_of_natural_origin)
{
}

AbsPolarStereoGraphicProj::~AbsPolarStereoGraphicProj() {}

// ------------------------------------------------------------------------

// Spherical Polar Stereo Graphic projection
// The transformation equations for a sphere of radius R are given by
//    x  =  k.cos(phi).sin(lambda-lambda_0)                                    (1)
//    y  =  k.[cos(phi_1).sin(phi)-sin(phi_1).cos(phi).cos(lambda-lambda_0)]   (2)
//
//    where lambda_0 is the central longitude, phi_1 is the central latitude, and
//
//    k=(2R)/(1+sin(phi_1).sin(phi) + cos(phi_1).cos(phi).cos(lambda-lambda_0)) (3)
//
// The inverse formulas for latitude phi and longitude lambda are then given by:
//
//   phi      =  sin^(-1)(cos(c)sin(phi_1)+(y.sin(c)cos(phi_1))/rho)   (4)
//   lambda   =  lambda_0+tan^(-1)((x.sin(c)/(rho.cos(phi_1).cos(c)-y.sin(phi_1).sin(c))), (5)
//
// where
//    rho   =  sqrt(x^2+y^2)     (6)
//    c  =  2tan^(-1)(rho/(2R))  (7)
//
// and the two-argument form of the inverse tangent function is best used for this computation.
//
// For an oblate spheroid, R can be interpreted as the "local radius," defined by
//    R=(R_e.cos(phi))/((1-e^2.sin^2(phi))cos(chi)),    (8)
//
// where R_e is the equatorial radius and chi is the conformal latitude.
//
// All projection suffer from distortion. The standard parallel, is where there is least amount of distortion.
// This can be used to come up with a scaling factor.
// i.e circum @ 60 lati on earth / circum @ 60 latitude on projection
//

SphericalPolarStereoGraphicProj::SphericalPolarStereoGraphicProj(
  bool southPoleOnProjectionPlane,
  long lon_of_natural_origin
)
:AbsPolarStereoGraphicProj(southPoleOnProjectionPlane,lon_of_natural_origin)
{
   double phi_1 = 0;                               // central latitude
   if ( southPoleOnProjectionPlane )
      phi_1 = -90.0;
   else
      phi_1 = 90.0;

   cos_phi1_ = cos( phi_1  * degree_to_radian_);
   sin_phi1_ = sin( phi_1  * degree_to_radian_);
}

eckit::geometry::Point2 SphericalPolarStereoGraphicProj::map_to_plane( const eckit::geometry::LLPoint2& point) const
{
   double cos_phi = cos( point.lat()  * degree_to_radian_);
   double sin_phi = sin( point.lat()  * degree_to_radian_);

   double lambda_0 = lon_of_natural_origin_ * degree_to_radian_;
   double lambda = point.lon() * degree_to_radian_;
   double lambda_diff = lambda - lambda_0;

   double cos_lambda_diff = cos( lambda_diff );
   double sin_lambda_diff = sin( lambda_diff );

   //    k=(2R)/(1+sin(phi_1).sin(phi) + cos(phi_1).cos(phi).cos(lambda-lambda_0)) (3)
   double k = 2.0 * radius_ / (1.0 + sin_phi1_*sin_phi + cos_phi1_*cos_phi*cos_lambda_diff) ;

   //      x = k.cos(phi).sin(lambda-lambda_0)                                    (1)
   double x = k * cos_phi* sin_lambda_diff;

   //      y = k.[cos(phi_1).sin(phi)-sin(phi_1).cos(phi).cos(lambda-lambda_0)]   (2)
   double y = k*(cos_phi1_*sin_phi - sin_phi1_*cos_phi*cos_lambda_diff);

   return eckit::geometry::Point2(x,y);
}

eckit::geometry::LLPoint2 SphericalPolarStereoGraphicProj::map_to_spherical( double x, double y) const
{
   //      rho = sqrt(x^2+y^2)     (6)
   double rho = sqrt(x*x + y*y);
   if (rho == 0) {
      return eckit::geometry::LLPoint2(0,0);
   }

   //      c = 2.tan^(-1)(rho/(2R))  (7)
   double c = 2.0 * atan( rho/(2.0 * radius_));

   //       lat  =  sin^(-1)(cos(c).sin(phi_1) + (y.sin(c).cos(phi_1))/rho )   (4)
   double cos_c = cos(c);
   double sin_c = sin(c);

   // Uses arc sin, to convert back to degrees, put in range -1 to 1 in case of slight rounding error
   // avoid error on calculating e.g. asin(1.00000001)
   double z = cos_c * sin_phi1_ + ((y * sin_c * cos_phi1_)/rho) ;
   if (z > 1.0)  z = 1.0;
   if (z < -1.0) z = -1.0;
   double lat = asin( z ) * radian_to_degree_;

   //   lon   =  lambda_0+tan^(-1)((x.sin(c)/(rho.cos(phi_1).cos(c)-y.sin(phi_1).sin(c))), (5)
   double lambda_0 = lon_of_natural_origin_ * degree_to_radian_;              // central longitude
   double lon = (lambda_0 + atan2( (x * sin_c) , (rho * cos_phi1_ * cos_c - y * sin_phi1_ * sin_c) )) * radian_to_degree_ ;

   // Still get a very small rounding error, round to 6 decimal places
   lat = roundf( lat * 1000000.0 )/1000000.0;
   lon = roundf( lon * 1000000.0 )/1000000.0;

   return eckit::geometry::LLPoint2(lon,lat);
}

//-------------------------------------------------------------------------------------------------------------
// Ellipsoid
// ==========
// Conformal latitude for a ellipse
//  X = 2.arctan[ tan( 180/4 + lat/2).[( 1 - e.sin(lat)/(1 + e.sind(lat)] pow(e/2) } - 180/2;
//
//  a = 6378137.0m for  WGS 84
//  e = 0.081819191
//  1/f = 298.2572236
//  Ko = 0.994    central scale factor
//
//  lonc = cental logitude.
//  t = tan(180/4 - lat/2)/ { [ ( 1 - e.sin(lat))/( 1 + e.sin(lat))] pow(e/2) }
//  p = 2.a.Ko.t/ { sqrt([( 1 + e)pow(1+e) .( 1 - e)pow(1-e)])
//  x = p.sin(lon - Lonc);
//  y = p.cos(lon - lonc); // south pole
//  y = -p.cos(lon - lonc); // north pole
//

EllipPolarStereoGraphicProj::EllipPolarStereoGraphicProj(
  bool southPoleOnProjectionPlane,
  long lon_of_natural_origin
)
: AbsPolarStereoGraphicProj(southPoleOnProjectionPlane,lon_of_natural_origin),
  Ko_(0.994),
  e_(0.081819191),
  fe_(0),
  fn_(0)
{
   radius_ = 6378137;
}

eckit::geometry::Point2 EllipPolarStereoGraphicProj::map_to_plane( const eckit::geometry::LLPoint2& point) const
{
   double lat = point.lat();
   double sin_lat = sin(lat * degree_to_radian_);
   double t = tan( (45.0 - lat/2) * degree_to_radian_ ) / pow((1 - e_ * sin_lat)/ ( 1 + e_ * sin_lat), e_/2);

   double a = radius_;
   double p = (2 * a * Ko_ * t) / sqrt( ( pow(1+e_,1+e_) * pow(1-e_,1-e_)) ) ;

   double lon = point.lon() * degree_to_radian_;
   double lonc = lon_of_natural_origin_ * degree_to_radian_;

   double x = fe_ + p * sin( lon - lonc );
   double y;
   if (southPoleOnProjectionPlane_) {
      y = fn_ + p * cos( lon - lonc );
   }
   else {
      y = fn_ + -( p * cos( lon - lonc ));
   }

   return eckit::geometry::Point2(x,y);
}

// See: Map Projections a Working Manual
// Ko = 0.994    scale factor at natural origin
// a = 6378137.0m for  WGS 84
// e = 0.081819191
// p' = sqrt(x*x + y*y)
// t' = p' . { sqrt([ (1 + e) pow(1+e) . ( 1 - e)pow(1-e) }  / 2 .a.Ko
// X = 2.atan(t') - pi/2     # south pole case
// X = pi/2 - 2.atan(t')     # north pole case
//
// lat =  X + (e**2/2    + 5e**4/24 + e**6/12 + 13e**8/360).sin(2.X))
//          + (7e**4/48  + 29e**6/240 + 811e**8/11520).sin(4.X)
//          + (7e**6/120 + 81e**8/1120).sin(6.X)
//          + (4279e**8/161280).sin(8.X)
//
// Note: while ratio of x/y is same, for north/south pole, atan2 takes into account the quadrant
// lon = Lon0 + atan( x/y)    # south pole  lon = Lon0 + atan2( x,y)
// lon = Lon0 + atan( x/-y)   # north pole  lon = Lon0 + atan2( x,-y)
eckit::geometry::LLPoint2 EllipPolarStereoGraphicProj::map_to_spherical( double E, double N) const
{
   // Note: to avoid integer division, one must be cast to a double, i.e a(double)/b  || a/(double)b
   double a = radius_;
   double pd = sqrt( (E-fe_)*(E-fe_) + (N-fn_)*(N-fn_) );
   double td = pd * sqrt( pow(1+e_,1+e_) * pow(1-e_,1-e_)  ) / (double) (2.0*a*Ko_) ;

   double X = 0;
   if (southPoleOnProjectionPlane_) {
      X = 2.0 * atan(td) - 90.0 * degree_to_radian_;
   }
   else {
      X = 90. * degree_to_radian_  - 2. * atan(td) ;
   }

   double lat =  X + (e_*e_/2.0    +  5*e_*e_*e_*e_/24.  + pow(e_,6)/12. + 13*pow(e_,8)/360.0) * sin(2.0 * X)
                    + (7*pow(e_,4)/48. + 29*pow(e_,6)/240. + 811*pow(e_,8)/11520.)  * sin(4.0 * X)
                    + (7*pow(e_,6)/120.  + 81*pow(e_,8)/1120.)  * sin(6.0 * X)
                    + (4279*pow(e_,8)/161280.)  * sin(8.0 * X) ;
   lat *= radian_to_degree_;


   double lon;
   if (southPoleOnProjectionPlane_) {
      lon = lon_of_natural_origin_ + atan2((E-fe_),(N-fn_)) * radian_to_degree_;
   }
   else {
      lon = lon_of_natural_origin_ + atan2((E-fe_),fn_-N) * radian_to_degree_;
   }

   return eckit::geometry::LLPoint2(lon,lat);
}

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

