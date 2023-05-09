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

#include <ostream>
#include <variant>
#include <vector>

#include "eckit/geo/types/PointLatLon.h"
#include "eckit/geo/types/PointXY.h"
#include "eckit/geo/types/PointXYZ.h"


namespace eckit::geo {


using PointLatLon = types::PointLatLon<double>;
using PointXY     = types::PointXY<double>;
using PointXYZ    = types::PointXYZ<double>;
using Point       = std::variant<PointLatLon, PointXY, PointXYZ>;


using pl_type = std::vector<long>;


}  // namespace grit

std::ostream &operator<<(std::ostream &, const eckit::geo::Point &);
