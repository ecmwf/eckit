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

#include "eckit/geometry/Point2.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry::polygon {

//----------------------------------------------------------------------------------------------------------------------

class LonLatPolygon : protected std::vector<Point2> {
public:

    // -- Types
    using container_type = std::vector<Point2>;
    using container_type::value_type;

    // -- Constructors

    explicit LonLatPolygon(const container_type& points, bool includePoles = true);

    template <typename Point2Iterator>
    LonLatPolygon(Point2Iterator begin, Point2Iterator end, bool includePoles = true) :
        LonLatPolygon(container_type(begin, end), includePoles) {}

    LonLatPolygon(const LonLatPolygon&) = default;
    LonLatPolygon(LonLatPolygon&&)      = default;

    // -- Destructor

    virtual ~LonLatPolygon() = default;

    // -- Operators

    LonLatPolygon& operator=(const LonLatPolygon&) = default;
    LonLatPolygon& operator=(LonLatPolygon&&)      = default;

    // -- Methods

    const Point2& max() const { return max_; }
    const Point2& min() const { return min_; }

    using container_type::operator[];
    using container_type::size;

    /// @brief Point-in-polygon test based on winding number
    /// @note reference <a href="http://geomalgorithms.com/a03-_inclusion.html">Inclusion of a Point in a Polygon</a>
    /// @param[in] P given point
    /// @param[in] normalise_angle normalise point angles
    /// @return if point (lon,lat) is in polygon
    bool contains(const Point2& Plonlat, bool normalise_angle = false) const;

private:

    // -- Methods

    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const LonLatPolygon&);

    // -- Members

    Point2 max_;
    Point2 min_;
    bool includeNorthPole_;
    bool includeSouthPole_;
    bool quickCheckLongitude_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry::polygon
