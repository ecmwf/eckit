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

#include <utility>
#include <vector>

#include "mir/util/Types.h"


namespace mir::util {


class Polygon2 : public std::vector<Point2> {
public:
    using container_type = std::vector<Point2>;

    Polygon2(const container_type& points) : container_type(points) {}
    Polygon2(container_type&& points) : container_type(std::move(points)) {}

    using container_type::container_type;

    void clip(const Polygon2&);
    void simplify();
    double area() const;
    Point2 centroid() const;

    bool operator==(const Polygon2&) const;

private:
    using Edge = std::pair<const Point2&, const Point2&>;

    Edge edge(int i) const;
    void emplace_back_point(Point2);
    void emplace_back_point_at_intersection(const Edge&, const Edge&);
};


}  // namespace mir::util
