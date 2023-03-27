/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "grit/Transformation.h"


namespace grit {


#if 0
- mercator:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double projectionOrientationInDegrees
    size_t n_i
    size_t n_j
- polar_stereographic:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double projectionOrientationInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- lambert:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double Latin1InDegrees
    double Latin2InDegrees
    double LoVInDegrees
    double latitudeOfFirstPointInDegrees
    double latitudeOfSouthernPoleInDegrees
    double longitudeOfFirstPointInDegrees
    double longitudeOfSouthernPoleInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- albers:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double Latin1InDegrees
    double Latin2InDegrees
    double LoVInDegrees
    double latitudeOfFirstPointInDegrees
    double latitudeOfSouthernPoleInDegrees
    double longitudeOfFirstPointInDegrees
    double longitudeOfSouthernPoleInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- space_view:
    double DiInMetres
    double DjInMetres
    double altitudeFromEarthCentreInMetres
    double latitudeOfSubSatellitePointInDegrees
    double longitudeOfSubSatellitePointInDegrees
    double projectionOrientationInDegrees
    double xOriginOfSectorImageInMetres
    double xSubSatellitePointInMetres
    double yOriginOfSectorImageInMetres
    double ySubSatellitePointInMetres
    size_t n_i
    size_t n_j
- lambert_azimuthal_equal_area:
    double DiInMetres
    double DjInMetres
    double centralLongitudeInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double standardParallelInDegrees
- equatorial_azimuthal_equidistant:
    double DiInMetres
    double DjInMetres
    double latitudeOfTangencyPointInDegrees
    double longitudeOfTangencyPointInDegrees
    flags projectionCentreFlag
- unstructured_grid (grib2/localConcepts/[centre:s]/unstructuredGrid*):
    string unstructuredGridSubtype = undefined/T/U/V/W/F (numberOfGridInReference)
    string unstructuredGridType = undefined/ORCA2/ORCA1/ORCA025/ORCA12/eORCA1/eORCA025/eORCA12 (numberOfGridInReference)
    string uuidOfHGrid
- irregular_latlon

Projection, rotation, shape (grib2/template.3.(rotation|shape_of_the_earth).def)
- projSourceString, projTargetString, projString=projTargetString

    double latitudeOfSouthernPoleInDegrees
    double longitudeOfSouthernPoleInDegrees
    double angleOfRotationInDegrees

    bool is_oblate
    double majorAxisInMetres
    double minorAxisInMetres
    double radiusInMetres

Projection centre (grib2/tables/30/3.5.table)
  1 0 North Pole is on the projection plane
  1 1 South Pole is on the projection plane
  2 0 Only one projection centre is used
  2 1 Projection is bipolar and symmetric
#endif


}  // namespace grit
