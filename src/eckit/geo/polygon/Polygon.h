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

#include "eckit/geo/PointLonLat.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo::polygon {


class Polygon : public std::vector<PointLonLat> {
public:

    // -- Types

    using container_type = vector;
    using container_type::value_type;

    // -- Constructors

    using container_type::container_type;

    explicit Polygon(const container_type& points) : container_type(points) {}
    explicit Polygon(container_type&& points) : container_type(std::move(points)) {}

    Polygon(const Polygon&) = default;
    Polygon(Polygon&&)      = default;

    // -- Destructor

    ~Polygon() = default;

    // -- Operators

    Polygon& operator=(const Polygon&) = default;
    Polygon& operator=(Polygon&&)      = default;

    bool operator==(const Polygon&) const;
    bool operator!=(const Polygon& other) const { return !(*this == other); }

    // -- Methods

    using container_type::operator[];
    using container_type::size;

    /**
     * @brief Intersect polygon with bounding box
     * @param[in,out] bbox bounding box to intersect with, returns intersection
     * @return if polygon intersects bounding box
     */
    bool intersects(area::BoundingBox& bbox) const;

    /**
     * @brief Point-in-polygon test based on winding number
     * @param[in] P given point
     * @return if point is in polygon
     */
    bool contains(const PointLonLat& P) const;

    /**
     * @brief Centroid of polygon
     * @return centroid
     */
    PointLonLat centroid() const;

    /**
     * @brief Clip polygon with another polygon (commutative)
     * @param[in] other polygon to clip with
     */
    void clip(const Polygon&);

    /**
     * @brief Simplify polygon by removing consecutive and colinear points
     */
    void simplify();

    /**
     * @brief Area of polygon
     * @param[in] sign if true, returns positive area irrespective of point ordering
     * @return area respecting point ordering (positive on CCW point ordering)
     */
    double area(bool sign = false) const;

private:

    // -- Types

    using Edge = std::pair<const PointLonLat&, const PointLonLat&>;

    // -- Methods

    Edge edge(int) const;

    void emplace_back_point(PointLonLat);
    void emplace_back_point_at_intersection(const Edge&, const Edge&);

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream&, const Polygon&);
};


}  // namespace eckit::geo::polygon
