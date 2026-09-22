// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
