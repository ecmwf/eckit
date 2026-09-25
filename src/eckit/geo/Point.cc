// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Point.h"

#include <ostream>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo {


bool points_equal(const Point& p, const Point& q) {
    ASSERT(p.index() == q.index());
    return std::visit([&](const auto& p, const auto& q) { return points_equal(p, q); }, p, q);
}


bool points_equal(const Point& p, const Point& q, double eps) {
    ASSERT(p.index() == q.index());
    return std::visit([&](const auto& p, const auto& q) { return points_equal(p, q, eps); }, p, q);
}


std::ostream& operator<<(std::ostream& out, const Point& p) {
    std::visit([&](const auto& p) { out << p; }, p);
    return out;
}


template <>
const std::vector<std::string>& point_coordinates<PointXY>() {
    static const std::vector<std::string> names{"x", "y"};
    return names;
}


template <>
const std::vector<std::string>& point_coordinates<PointXYZ>() {
    static const std::vector<std::string> names{"x", "y", "z"};
    return names;
}


template <>
const std::vector<std::string>& point_coordinates<PointLonLat>() {
    static const std::vector<std::string> names{"longitude", "latitude"};
    return names;
}


template <>
const std::vector<std::string>& point_coordinates<PointLonLatR>() {
    static const std::vector<std::string> names{"longitude_r", "latitude_r"};
    return names;
}


}  // namespace eckit::geo
