/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "eckit/geometry/CoordinateHelpers.h"
#include "eckit/geometry/Point2.h"

namespace eckit {
namespace geometry {

//----------------------------------------------------------------------------------------------------------------------

double normalise_angle(double a, const double minimum) {
    while (a < minimum) {
        a += 360.;
    }
    while (a >= minimum + 360.) {
        a -= 360.;
    }
    return a;
}

//----------------------------------------------------------------------------------------------------------------------

Point2 canonicaliseOnSphere(const Point2& lonlat, const double minimum_lon) {
    const double lat = normalise_angle(lonlat[1], -90.);
    const bool across_pole = (lat > 90.);

    if (!across_pole) {
        return Point2(normalise_angle(lonlat[0], minimum_lon), lat);
    } else {
        return Point2(normalise_angle(lonlat[0] + 180., minimum_lon), 180. - lat);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit
