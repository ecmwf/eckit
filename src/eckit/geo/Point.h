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

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointLonLatR.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/PointXYZ.h"


namespace eckit::geo {


using Point = std::variant<std::monostate, PointXY, PointXYZ, PointLonLat, PointLonLatR>;

bool points_equal(const Point&, const Point&);
bool points_equal(const Point&, const Point&, double eps);

std::ostream& operator<<(std::ostream&, const Point&);


}  // namespace eckit::geo
