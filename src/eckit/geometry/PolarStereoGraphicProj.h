#ifndef eckit_polarstereographicproj_h
#define eckit_polarstereographicproj_h

#include "eckit/geometry/Point2.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace geometry {

class PolarStereoGraphicProj {
public:
   // Set up transformations for Polar StereoGraphic grids
   // Assumes we have earth radius of 1.
   PolarStereoGraphicProj(
            bool north_pole_on_projection_plane,
            bool spherical_earth,               /* false means use oblate spheroid */
            long east_longitude                 /* Lov in grib talk */
            );

   /// The input is in spherical co-ordinates(lat,lon), and
   /// the return is mapped to projection plane(x,y)
   eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2&) const;

   /// The input is on projection plane, and output is return back to spherical co-ordinates
   eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const;


   /// Set the scale factor. at natural origin, only used for ellipsoid
   /// = 0.994          for oblate spheroid
   void set_scale_factor_at_natural_origin(double Ko) { Ko_ = Ko;}

   /// Set the eccentricity for ellipsoid
   /// = 0.081819191 by default  WGS 84
   /// For ellipse  x*x/a*a + y*y/b*b = 1      e = sqrt(1 - b*b/a*a)
   /// Where a is emini-major axes and b = semi-minor axes
   void set_eccentricity(double e) { e_ = e; }

   /// Set the radius
   /// = 1.0 assumed for spherical earth
   /// = 63781370    for Ellipsoid WGS 84, i.e the equatorial radius
   void set_radius(double r) { radius_ = r; }

   // Bye default false nothing and easting set to 0
   void set_false_easting(double fe) { fe_ = fe ;}
   void set_false_northing(double fn) { fn_ = fn ;}

private:
   // assume earth is spherical
   eckit::geometry::Point2 sp_map_to_plane( const eckit::geometry::LLPoint2&) const;
   eckit::geometry::LLPoint2 sp_map_to_spherical( double x, double y) const;

   // assume earth is oblate spherical
   eckit::geometry::Point2 el_map_to_plane( const eckit::geometry::LLPoint2&) const;
   eckit::geometry::LLPoint2 el_map_to_spherical( double x, double y) const;

private:
   double degree_to_radian_;
   double radian_to_degree_;
   double radius_;
   double Ko_;
   double e_;
   double fe_;
   double fn_;

   bool north_pole_on_projection_plane_;
   bool spherical_earth_;
   long east_longitude_;

   double cos_phi1_; // cache
   double sin_phi1_; // cache
};

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
