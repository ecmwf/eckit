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
#include <vector>

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"


namespace eckit::geo {


using PointLonLat = geometry::PointLonLat;
using Point2      = geometry::Point2;
using Point3      = geometry::Point3;
using Point       = std::variant<PointLonLat, Point2, Point3>;


using pl_type = std::vector<long>;


using geometry::points_equal;


bool points_equal(const Point&, const Point&);


}  // namespace eckit::geo


std::ostream& operator<<(std::ostream&, const eckit::geo::Point&);
