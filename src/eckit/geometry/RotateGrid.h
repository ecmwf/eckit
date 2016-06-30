#ifndef eckit_RotateGrid_h
#define eckit_RotateGrid_h

#include "eckit/geometry/Point2.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace geometry {

class RotateGrid {
public:
   // Set up rotated grid for transformations.
   // Inputs:
   //
   // south_pole : latitude (degrees) and longitude (degrees) of the
   //              pole of the rotated grid, as seen in the non-rotated grid
   //
   // polerotate: optional input -- by default, the calculation assumes
   //             that the rotated grid is singly rotated, i.e. that the
   //             common meridian which passes through poles of rotated
   //             and non-rotated grid has the same longitude value in
   //             both grids.  If there is additional rotation about the
   //             pole of the rotated grid, then set this input to the
   //             value in degrees
   //
   // lonMin:  minimum longitude for output of rotate to be performed
   //          defaults to 0 so that longitudes are normally output in
   //          the range [0 360] but e.g. specify as -180 if [-180, 180] is desired.
   //
   // Assumes earth is a sphere. (if you need it as a ellipsoid, this will not suffice)
   RotateGrid(const eckit::geometry::LLPoint2& south_pole,
           double polerotate = 0,
           double lonMin = -180); // for -180 - +180, choose lonMin as -180

   // magics, assume south_pole_rot_angle_ = 0; *** Used for test comparison ***
   eckit::geometry::LLPoint2 magics_rotate( const eckit::geometry::LLPoint2&) const;
   eckit::geometry::LLPoint2 magics_unrotate( const eckit::geometry::LLPoint2&) const;

   // Input point is in the un-rotated grid. returns point in the rotated grid
   eckit::geometry::LLPoint2 rotate( const eckit::geometry::LLPoint2&) const;

   // Input is in the rotated grid, return back to un-rotated grid
   eckit::geometry::LLPoint2 unrotate( const eckit::geometry::LLPoint2&) const;

private:
   double degree_to_radian_;
   double radian_to_degree_;

   eckit::geometry::LLPoint2 south_pole_;
   double south_pole_rot_angle_;
   double lonmin_;
   double lonmax_;
};

// -----------------------------------------------------------------------------------

//RotgridPy *** Used for test comparison only****
// See; http://proj.badc.rl.ac.uk/svn/cows/cows_support/rotated_grid_utils/trunk/lib/rotated_grid_utils/rotgrid.py
//
// Contains routines to calculate the position of a given point as seen in a rotated grid.
//
// The easiest interface is by instantiating RotgridPy and calling the transform() method.
//
// For repeated calculations with the same lats and lons (e.g. transforming
// every point on the non-rotated grid), it may be more efficient to call
// rotgrid_core directly.  This requires some terms to have been
// pre-calculated by the calling routine.

class RotgridPy {
public:
   // Set up rotated grid for transformations.
   //        Inputs:
   //
   //        south_pole_lon, south_pole_lat: longitude (degrees) and latitude (degrees) of the
   //                          pole of the rotated grid, as seen in the
   //                          non-rotated grid
   //
   //        polerotate: optional input -- by default, the calculation assumes
   //                    that the rotated grid is singly rotated, i.e. that the
   //                    common meridian which passes through poles of rotated
   //                    and non-rotated grid has the same longitude value in
   //                    both grids.  If there is additional rotation about the
   //                    pole of the rotated grid, then set this input to the
   //                    value in degrees
   //
   //        nPoleGridLon: an alternative way of specifying the longitudinal
   //                      rotation between grids: specify as the longitude
   //                      (degrees) of the true north pole as seen in the
   //                      rotated grid.  If set, overrides polerotate.
   //
   //        lonMin:  minimum longitude for output of transforms to be perfomed
   //                 defaults to -180 so that longitudes are normally output in
   //                 the range [-180, 180) but e.g. specify as 0 if [0, 360) is
   //                 desired.
   RotgridPy(double south_pole_lat, double south_pole_lon,
           double polerotate = 0,
           double nPoleGridLon = 0,
           double lonMin = 0); // for -180 - +180, choose lonMin as -180

   // Performs transformations to/from rotated grid.
   //
   //        Inputs:
   //
   //          lon, lat: longitude (degrees) and latitude (degrees)
   //                    of a point X, as seen in the non-rotated grid
   //
   //          inverse: optional input -- set to a true value for inverse transform
   //                          (coords on rotated grid to coords on nonrotated)
   //
   //        Returns:
   //
   //            The coordinates of the point X (in degrees) as seen in the rotated
   //            grid (or the non-rotated grid in case of inverse transform), as a
   //            2-element tuple: (longitude, latitude)
   std::pair<double,double> transform(double lat, double lon, bool inverse = false) const;

private:
   // Inputs:
   //
   //      cossouth_pole_lat, sinsouth_pole_lat:
   //            cos and sine of latitude of the pole
   //            of the rotated grid, as seen in the non-rotated grid
   //
   //      sindlon, cosdlon, coslat, sinlat:
   //            cos and sine of longitude offset
   //            and cos and sine of latitude
   //            of a point X, as seen in the non-rotated grid
   //
   //            (NB longitude offset is taken from the common meridian which
   //            passes through poles of rotated and non-rotated grid)
   //
   //    Returns:
   //
   //      The coordinates of the point X (in radians) as seen in the rotated grid.
   //      as a 2-element tuple: (longitude offset, latitude)
   //
   //      (NB longitude offset is taken from the common meridian which
   //      passes through poles of the rotated of non-rotated grids)
   std::pair<double,double> rotgrid_core(
               double cossouth_pole_lat, double sinsouth_pole_lat,
               double cosdlon, double sindlon, double coslat, double sinlat) const;

private:
   double degree_to_radian_;
   double radian_to_degree_;

   double south_pole_lat_;
   double south_pole_lon_;
   double south_pole_rot_angle_;

   double cossouth_pole_lat_;
   double sinsouth_pole_lat_;
   double lonmin_;
   double lonmax_;
};

//---------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
