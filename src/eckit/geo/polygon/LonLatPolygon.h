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
#include <vector>

#include "eckit/geo/PointLonLat.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo::polygon {


class LonLatPolygon : protected std::vector<PointLonLat> {
public:

    // -- Types

    using container_type = vector;
    using container_type::value_type;

    // -- Constructors

    explicit LonLatPolygon(const container_type& points, bool includePoles = true);

    template <typename Iterator>
    LonLatPolygon(Iterator begin, Iterator end, bool includePoles = true) :
        LonLatPolygon(container_type(begin, end), includePoles) {}

    LonLatPolygon(const LonLatPolygon&) = default;
    LonLatPolygon(LonLatPolygon&&)      = default;

    // -- Destructor

    virtual ~LonLatPolygon() = default;

    // -- Operators

    LonLatPolygon& operator=(const LonLatPolygon&) = default;
    LonLatPolygon& operator=(LonLatPolygon&&)      = default;

    // -- Methods

    const PointLonLat& max() const { return max_; }
    const PointLonLat& min() const { return min_; }

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
     * @param[in] normalise_angle normalise point angles
     * @return if point (lon,lat) is in polygon
     */
    bool contains(const PointLonLat& P, bool normalise_angle = false) const;

    /**
     * @brief Area of polygon
     * @return area respecting point ordering (positive on CCW point ordering)
     */
    double area() const;

private:

    // -- Methods

    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const LonLatPolygon&);

    // -- Members

    PointLonLat max_;
    PointLonLat min_;
    bool includeNorthPole_;
    bool includeSouthPole_;
    bool quickCheckLongitude_;
};


}  // namespace eckit::geo::polygon
