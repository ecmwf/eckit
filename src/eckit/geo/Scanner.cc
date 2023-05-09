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


#include "eckit/geo/Scanner.h"


namespace eckit::geo {


#if 0
Scanning mode (grib2/tables/30/3.4.table, grib2/template.3.scanning_mode.def)

    bool iScansPositively
- 1 0 Points of first row or column scan in the +i (+x) direction
- 1 1 Points of first row or column scan in the -i (-x) direction

    bool jScansPositively
- 2 0 Points of first row or column scan in the -j (-y) direction
- 2 1 Points of first row or column scan in the +j (+y) direction

    bool jPointsAreConsecutive
- 3 0 Adjacent points in i (x) direction are consecutive
- 3 1 Adjacent points in j (y) direction is consecutive

    bool alternativeRowScanning
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

#endif


}  // namespace eckit::geo
