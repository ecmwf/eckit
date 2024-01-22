/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <limits>
#include <sstream>
#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/CoordinateHelpers.h"
#include "eckit/geometry/Point2.h"

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

inline double modulo(const double a, const double b) {
    return a-b*std::floor(a/b);
}

double normalise_angle(const double a, const double minimum) {
    return minimum + modulo(a-minimum, 360.);
}

//----------------------------------------------------------------------------------------------------------------------

Point2 canonicaliseOnSphere(const Point2& lonlat, const double minimum_lon) {
    const double lat       = normalise_angle(lonlat[1], -90.);
    const bool across_pole = (lat > 90.);

    if (!across_pole) {
        return {normalise_angle(lonlat[0], minimum_lon), lat};
    }

    return {normalise_angle(lonlat[0] + 180., minimum_lon), 180. - lat};
}

//----------------------------------------------------------------------------------------------------------------------

void assert_latitude_range(double lat) {
    if (!(-90. <= lat && lat <= 90.)) {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::max_digits10);
        oss << "Invalid latitude " << lat;
        throw BadValue(oss.str(), Here());
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry
