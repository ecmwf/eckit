/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
