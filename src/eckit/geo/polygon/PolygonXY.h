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


class PolygonXY : public std::vector<PointXY> {
public:

    // -- Types

    using container_type = vector;
    using container_type::value_type;

    // -- Constructors

    using container_type::container_type;

    explicit PolygonXY(const container_type& points) : container_type(points) {}
    explicit PolygonXY(container_type&& points) : container_type(std::move(points)) {}

    PolygonXY(const PolygonXY&) = default;
    PolygonXY(PolygonXY&&)      = default;

    // -- Destructor

    ~PolygonXY() = default;

    // -- Operators

    PolygonXY& operator=(const PolygonXY&) = default;
    PolygonXY& operator=(PolygonXY&&)      = default;

    bool operator==(const PolygonXY&) const;
    bool operator!=(const PolygonXY& other) const { return !(*this == other); }

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

    friend std::ostream& operator<<(std::ostream& s, const PolygonXY& p) {
        p.print(s);
        return s;
    }
};


}  // namespace eckit::geo::polygon
