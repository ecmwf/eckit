/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/CoordinateHelpers.h"
#include "eckit/geometry/Point2.h"

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

inline double modulo(const double a, const double b) {
    return a - b * std::floor(a / b);
}

double normalise_angle(double a, const double minimum) {
    constexpr double treshold = 4. * 360.;
    const double diff = a - minimum;
    if (std::abs(diff) > treshold) {
        // This formula is not bit-dentical with original below,
        // but is faster for very large values of 'a'.
        // The treshold tries to capture values of 'a' which need to
        // stay bit-identical.
        return minimum + modulo(diff, 360.);
    }

    // The original function
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
