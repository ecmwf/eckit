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


#pragma once

#include <iosfwd>
#include <variant>

#include "eckit/geo/Point2.h"
#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"


namespace eckit::geo {


using Point = std::variant<std::monostate, PointLonLat, Point2, Point3>;

bool points_equal(const Point&, const Point&);
bool points_equal(const Point&, const Point&, double eps);

std::ostream& operator<<(std::ostream&, const Point&);


constexpr double GLOBE        = 360.;
constexpr double GREENWICH    = 0.;
constexpr double ANTIMERIDIAN = -180.;
constexpr double EQUATOR      = 0.;
constexpr double NORTH_POLE   = 90.;
constexpr double SOUTH_POLE   = -90.;


// FIXME remove
using Longitude = double;
using Latitude  = double;


}  // namespace eckit::geo
