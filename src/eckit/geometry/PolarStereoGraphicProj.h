#ifndef eckit_polarstereographicproj_h
#define eckit_polarstereographicproj_h

#include "eckit/geometry/Point2.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace geometry {

// -----------------------------------------------------------------------------------
// The main class, that will be instantiated by the user:
//
// We support two variants, however others can be added:
//    o earth is sphere
//
//    o earth is oblate spheroid assumes following parameters apply:
//      - variant A: - Assumes WGS 84, for a,e by default, can be overridden
//                   - assumes latitude of natural origin is north/south pole (+/-90)
//                   - assumes longitude of natural origin is given, typically zero
//                   - scaling factor Ko at natural origin
//                   - false easting and false northing at natural origin
//
//      - There several other projection variants for the ellipsoid, but this appears to be
//        closest to the GRIB definition
//
// Usage:
// Example 1: Assume earth is spherical, project to south pole, longitude of natural origin is 0
//            First project on too the plane(x,y), then project back to the sphere(lat,lon)
//            We should get back same lat,lon
//
//    PolarStereoGraphicProj ps(false /* southPoleOnProjectionPlane */,
//                              false /* earth_is_oblate */,
//                              0     /* longitude of natural origin */);
//    double lat = 30;
//    double lon = 30;
//    eckit::geometry::Point2 pt_on_plane = ps.map_to_plane( eckit::geometry::LLPoint2(lat,lon) );
//    eckit::geometry::LLPoint2 pt_on_sphere = ps.map_to_spherical( pt_on_plane[0],  pt_on_plane[1] );
//    cout << "spherical(" << lat << "," << lon << ") -> pt_on_plane" << pt_on_plane
//         <<  " -> pt_on_sphere" << pt_on_sphere << "\n";
//
//
// Example 2: Assume earth is oblate spheroid: project to north pole, longitude of natural origin is 0:
//            WGS84, a=6378137, e=0.081819191,
//            First project unto the plane(x,y), then project back to the sphere(lat,lon)
//            We should get back same lat,lon
//
//   PolarStereoGraphicProj ps(false /* southPoleOnProjectionPlane */,
//                             true  /* earth_is_oblate            */,
//                             0     /* longitude of natural origin*/);
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

   PolarStereoGraphicProj(
            bool southPoleOnProjectionPlane,
            bool earth_is_oblate = false,     /* false means use earth as sphere */
            double lon_of_natural_origin  = 0    /* Lov/east latitude' in grib talk */
            );

   ~PolarStereoGraphicProj();

   /// Forward conversion:
   /// The input is in spherical co-ordinates(lat,lon), and
   /// the return is mapped to projection plane(x,y)
   eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2& p) const;

   /// Reverse conversion:
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

   /// Typically false easting and northing are used to avoid negative co-ordinates on the projection plane
   /// By default false nothing and easting set to 0
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
             double lon_of_natural_origin
             );

protected:
   double degree_to_radian_;
   double radian_to_degree_;
   double radius_;

   bool southPoleOnProjectionPlane_;
   double lon_of_natural_origin_;
};

// -----------------------------------------------------------------------------
// Earth is a perfect sphere
class SphericalPolarStereoGraphicProj : public AbsPolarStereoGraphicProj {
public:

   SphericalPolarStereoGraphicProj( bool southPoleOnProjectionPlane,
                                        double lon_of_natural_origin
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
             double lon_of_natural_origin
             );

   virtual ~EllipPolarStereoGraphicProj() {}

   virtual eckit::geometry::Point2 map_to_plane( const eckit::geometry::LLPoint2&) const;

   virtual eckit::geometry::LLPoint2 map_to_spherical( double x, double y) const;

   virtual void set_scale_factor_at_natural_origin(double Ko) { Ko_ = Ko;}

   virtual void set_eccentricity(double e) { e_ = e; }

   virtual void set_false_easting(double fe) { fe_ = fe ;}

   virtual void set_false_northing(double fn) { fn_ = fn ;}

protected:
   double Ko_;    // scale factor at natural origin, by default = 0.994, can be overridden
   double e_;     // eccentricity, by default = 0.081819191 WGS84, can be overridden
   double fe_;    // false easting at natural origin, default 0
   double fn_;    // false northing at natural origin,, default 0
};

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
