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

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"


namespace eckit::geo {


using geometry::Point2;
using geometry::Point3;
using geometry::PointLonLat;
using geometry::points_equal;


using Point = std::variant<PointLonLat, Point2, Point3>;
bool points_equal(const Point&, const Point&);


}  // namespace eckit::geo


std::ostream& operator<<(std::ostream&, const eckit::geo::Point&);