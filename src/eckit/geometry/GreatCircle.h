// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef GreatCircle_H
#define GreatCircle_H

#include <vector>
#include "eckit/geometry/Point2.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

class GreatCircle {
public:

    /// Great circle given two points in geographic coordinates
    GreatCircle(const Point2&, const Point2&);

    /// Great circle latitude given longitude, see http://www.edwilliams.org/avform.htm#Int
    std::vector<double> latitude(double lon) const;

    /// Great circle longitude given latitude, see http://www.edwilliams.org/avform.htm#Par
    std::vector<double> longitude(double lat) const;

    bool crossesPoles() const;

private:

    const Point2 A_;
    const Point2 B_;

    bool crossesPoles_;
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
