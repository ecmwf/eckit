/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <iosfwd>
#include <utility>
#include <vector>

#include "eckit/geo/PointXY.h"


namespace eckit::geo::polygon {


class Polygon2 : public std::vector<PointXY> {
public:

    // -- Types

    using container_type = vector;
    using container_type::value_type;

    // -- Constructors

    using container_type::container_type;

    explicit Polygon2(const container_type& points) : container_type(points) {}
    explicit Polygon2(container_type&& points) : container_type(std::move(points)) {}

    Polygon2(const Polygon2&) = default;
    Polygon2(Polygon2&&)      = default;

    // -- Destructor

    ~Polygon2() = default;

    // -- Operators

    Polygon2& operator=(const Polygon2&) = default;
    Polygon2& operator=(Polygon2&&)      = default;

    bool operator==(const Polygon2&) const;
    bool operator!=(const Polygon2& other) const { return !(*this == other); }

    // -- Methods

    /**
     * @brief Point-in-polygon test based on winding number
     * @param[in] P given point
     * @return if point is in polygon
     */
    bool contains(const PointXY& P) const;

    /**
     * @brief Simplify polygon by removing consecutive and colinear points
     */
    void simplify();

private:

    // -- Types

    using Edge = std::pair<const PointXY&, const PointXY&>;

    // -- Methods

    Edge edge(int) const;

    void emplace_back_point(PointXY);

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Polygon2& p) {
        p.print(s);
        return s;
    }
};


}  // namespace eckit::geo::polygon
