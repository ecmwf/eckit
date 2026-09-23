// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>
#include <vector>

#include "eckit/geo/PointLonLat.h"


namespace eckit::geo {


class GreatCircle {
public:

    /// Great circle given two points in geographic coordinates
    GreatCircle(const PointLonLat&, const PointLonLat&);

    /// Great circle latitude given longitude, see http://www.edwilliams.org/avform.htm#Int
    std::vector<double> latitude(double lon) const;

    /// Great circle longitude given latitude, see http://www.edwilliams.org/avform.htm#Par
    std::vector<double> longitude(double lat) const;

    /// If great circle crosses the poles (meridian/anti-meridian)
    bool crossesPoles() const;

    /**
     * @brief Calculate great circle course between two points
     *
     * @details Calculates the direction (clockwise from North) of a great circle arc between two points. Returns the
     * direction (angle) of the arc at each, normalised to the range of atan2 (usually (-180, 180]). All input and
     * output values are in units of degrees.
     *
     * @ref https://en.wikipedia.org/wiki/Great-circle_navigation
     */
    std::pair<double, double> course() const;

private:

    const PointLonLat A_;
    const PointLonLat B_;

    bool crossesPoles_;
};


}  // namespace eckit::geo
