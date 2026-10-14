// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Point.h"

#include <ostream>
#include <type_traits>

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


const std::vector<std::string>& point_coordinates(const Point& p) {
    if (std::holds_alternative<std::monostate>(p)) {
        throw BadValue("point_coordinates: invalid point (empty)", Here());
    }
    return point_coordinates(p.index());
}


namespace {


static_assert(std::is_same_v<std::variant_alternative_t<0, Point>, std::monostate>, "the empty point first");


template <size_t I = 1>
const std::vector<std::string>& point_coordinates_by_index(size_t index) {
    if constexpr (I < std::variant_size_v<Point>) {
        return index == I ? point_coordinates<std::variant_alternative_t<I, Point>>()
                          : point_coordinates_by_index<I + 1>(index);
    }
    else {
        throw BadValue("point_coordinates: invalid point type (" + std::to_string(index) + ")", Here());
    }
}


}  // namespace


const std::vector<std::string>& point_coordinates(size_t index) {
    return point_coordinates_by_index(index);
}


}  // namespace eckit::geo
