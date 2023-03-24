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


#include "grit/util.h"


namespace grit::util {


struct regular_ll_args {
    size_t n_i;
    size_t n_j;
    double i_start;
    double i_stop;
    double i_step;
    double j_start;
    double j_stop;
    double j_step;
};


}  // namespace grit::util


#if 0
gridType: gridDefinitionTemplateNumber
  - PLPresent: false
    - regular_ll
    - rotated_ll
    - regular_gg
    - rotated_gg

  - projections:
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
  - PLPresent: true
    - reduced_ll
    - reduced_gg
    - reduced_rotated_gg

Projection, rotation, shape (grib2/template.3.(rotation|shape_of_the_earth).def)
  - projSourceString, projTargetString, projString=projTargetString

  double latitudeOfSouthernPoleInDegrees
  double longitudeOfSouthernPoleInDegrees
  double angleOfRotationInDegrees

  bool is_oblate
  double majorAxisInMetres
  double minorAxisInMetres
  double radiusInMetres

Scanning mode (grib2/tables/30/3.4.table, grib2/template.3.scanning_mode.def)
  - 1 0 Points of first row or column scan in the +i (+x) direction
  - 1 1 Points of first row or column scan in the -i (-x) direction
  - 2 0 Points of first row or column scan in the -j (-y) direction
  - 2 1 Points of first row or column scan in the +j (+y) direction
  - 3 0 Adjacent points in i (x) direction are consecutive
  - 3 1 Adjacent points in j (y) direction is consecutive
  - 4 0 All rows scan in the same direction
  - 4 1 Adjacent rows scan in the opposite direction
  - 5 0 Points within odd rows are not offset in i (x) direction
  - 5 1 Points within odd rows are offset by Di/2 in i (x) direction
  - 6 0 Points within even rows are not offset in i (x) direction
  - 6 1 Points within even rows are offset by Di/2 in i (x) direction
  - 7 0 Points are not offset in j (y) direction
  - 7 1 Points are offset by Dj/2 in j (y) direction
  - 8 0 n_i columns, n_j rows
  - 8 1 Rows have n_i points if points are not offset in i direction
        Rows have n_i-1 points if points are offset by Di/2 in i direction
        Columns have n_j points if points are not offset in j direction
        Columns have n_j-1 points if points are offset by Dj/2 in j direction

  bool iScansPositively
  bool jScansPositively
  bool jPointsAreConsecutive
  bool alternativeRowScanning

Projection centre (grib2/tables/30/3.5.table)
  1 0 North Pole is on the projection plane
  1 1 South Pole is on the projection plane
  2 0 Only one projection centre is used
  2 1 Projection is bipolar and symmetric

(grib2/template.3.(gaussian|grid|latlon).def)
  double latitudeOfFirstPointInDegrees
  double latitudeOfLastPointInDegrees
  double longitudeOfFirstPointInDegrees
  double longitudeOfLastPointInDegrees
  double DiInDegrees
  double DjInDegrees
  size_t n_i
  size_t n_j
  size_t numberOfParallelsBetweenAPoleAndTheEquator
  size_t basicAngleOfTheInitialProductionDomain
  size_t subdivisionsOfBasicAngle
#endif

