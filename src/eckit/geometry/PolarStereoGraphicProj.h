#ifndef eckit_polarstereographicproj_h
#define eckit_polarstereographicproj_h

#include "eckit/geometry/Point2.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace geometry {

// -----------------------------------------------------------------------------------
// The main class, that will be instantiated by the user
//
// Usage:
// Example 1: Assume earth is spherical, project to south pole, east longitude is 0
//            First project unto the plane(x,y), then project back to the sphere(lat,lon)
//            We should get back same lat,lon
//
//    PolarStereoGraphicProj ps(false /*southPoleOnProjectionPlane*/,
//                              false /*earth_is_oblate*/,
//                              0     /*east longitude*/);
//    double lat = 30;
//    double lon = 30;
//    eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lat,lon) );
//    eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[0],  pt_on_plane[1] );
//    cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane
//         <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
//
// Example 2: Assume earth is oblate spheroid: project to north pole, east longitude is 0:
//            WGS84, a=6378137, e=0.081819191, Ko=0.994
//            First project unto the plane(x,y), then project back to the sphere(lat,lon)
//            We should get back same lat,lon
//
//   PolarStereoGraphicProj ps(false /*southPoleOnProjectionPlane*/,
//                             true /*earth_is_oblate*/,
//                             0    /*east longitude*/);
//   ps.set_false_easting( 2000000);
//   ps.set_false_northing(2000000);
//   double lat = 73;
//   double lon = 44;
//   eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lat,lon) );
//   eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[0],  pt_on_plane[1] );
//   cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane
//        <<  " -> pt_on_sphere" << pt_on_sphere << "\n";

class AbsPolarStereoGraphicProj;

class PolarStereoGraphicProj : private eckit::NonCopyable {
public:
   // Set up transformations for Polar StereoGraphic grids
   // default Assumes we have earth radius of 1.
   PolarStereoGraphicProj(
            bool southPoleOnProjectionPlane,
            bool earth_is_oblate = false,      /* false means use shere */
            long east_longitude  = 0           /* Lov in grib talk */
            );

   ~PolarStereoGraphicProj();

   /// The input is in spherical co-ordinates(lat,lon), and
   /// the return is mapped to projection plane(x,y)
   eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2& p) const;

   /// The input is on projection plane, and output is return back to spherical co-ordinates
   eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const;

   /// Set the radius
   /// = 1.0 assumed for spherical earth
   /// = 63781370    for Ellipsoid WGS 84, i.e the equatorial radius
   void set_radius(double r);

   /// Set the scale factor. at natural origin, only used for ellipsoid
   /// = 0.994          for oblate spheroid
   void set_scale_factor_at_natural_origin(double Ko);

   /// Set the eccentricity for ellipsoid
   /// = 0.081819191 by default  WGS 84
   /// For ellipse  x*x/a*a + y*y/b*b = 1      e = sqrt(1 - b*b/a*a)
   /// Where a is semi-major axes and b = semi-minor axes
   void set_eccentricity(double e);

   // By default false nothing and easting set to 0
   void set_false_easting(double fe);
   void set_false_northing(double fn);

private:
   ScopedPtr<AbsPolarStereoGraphicProj> proj_;
};


// ----------------------------------------------------------------------------------------
// Base class, for creating different variants of polar stereo graphic projections
// ----------------------------------------------------------------------------------------
class AbsPolarStereoGraphicProj : private eckit::NonCopyable {
public:
   virtual ~AbsPolarStereoGraphicProj();

   virtual eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2&) const = 0;

   virtual eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const = 0;

   void set_radius(double r) { radius_ = r;}

   virtual void set_scale_factor_at_natural_origin(double Ko){};

   virtual void set_eccentricity(double e){}

   virtual void set_false_easting(double fe){}

   virtual void set_false_northing(double fn){}

protected:

   AbsPolarStereoGraphicProj(
             bool southPoleOnProjectionPlane,
             long east_longitude                 /* Lov in grib talk */
             );

protected:
   double degree_to_radian_;
   double radian_to_degree_;
   double radius_;

   bool southPoleOnProjectionPlane_;
   long east_longitude_;
};

// -----------------------------------------------------------------------------
// Earth is a perfect sphere
class SphericalPolarStereoGraphicProj : public AbsPolarStereoGraphicProj {
public:

   SphericalPolarStereoGraphicProj( bool southPoleOnProjectionPlane,
                                        long east_longitude                 /* Lov in grib talk */
                                       );

   virtual ~SphericalPolarStereoGraphicProj() {}

   virtual eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2&) const;

   virtual eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const;

private:

   double cos_phi1_; // cache
   double sin_phi1_; // cache
};

// -----------------------------------------------------------------------------
// Earth is a oblate spheroid
class EllipPolarStereoGraphicProj : public AbsPolarStereoGraphicProj {
public:

   EllipPolarStereoGraphicProj(
             bool southPoleOnProjectionPlane,
             long east_longitude                 /* Lov in grib talk */
             );

   virtual ~EllipPolarStereoGraphicProj() {}

   virtual eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2&) const;

   virtual eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const;

   virtual void set_scale_factor_at_natural_origin(double Ko) { Ko_ = Ko;}

   virtual void set_eccentricity(double e) { e_ = e; }

   virtual void set_false_easting(double fe) { fe_ = fe ;}

   virtual void set_false_northing(double fn) { fn_ = fn ;}

protected:
   double Ko_;    // scale factor at natural origin, by deault = 0.994, can be overridden
   double e_;     // eccentricity, by default = 0.081819191, WGS84 can be overridden
   double fe_;    // false easting
   double fn_;    // false northing
};

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
