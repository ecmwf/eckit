// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <variant>
#include <vector>

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointLonLatR.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/PointXYZ.h"


namespace eckit::geo {


using Point = std::variant<std::monostate, PointXY, PointXYZ, PointLonLat, PointLonLatR>;


/// Point comparison functions
bool points_equal(const Point&, const Point&);
bool points_equal(const Point&, const Point&, double eps);


std::ostream& operator<<(std::ostream&, const Point&);


/// Coordinate names of point type P, eg. ("longitude", "latitude") (one per coordinate)
template <typename P>
const std::vector<std::string>& point_coordinates();

template <>
const std::vector<std::string>& point_coordinates<PointXY>();

template <>
const std::vector<std::string>& point_coordinates<PointXYZ>();

template <>
const std::vector<std::string>& point_coordinates<PointLonLat>();

template <>
const std::vector<std::string>& point_coordinates<PointLonLatR>();

const std::vector<std::string>& point_coordinates(const Point&);


/// Coordinate names of the point type at an index of Point
const std::vector<std::string>& point_coordinates(size_t index);


}  // namespace eckit::geo
