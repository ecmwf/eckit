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

namespace eckit {
namespace geometry {
namespace polygon {

//----------------------------------------------------------------------------------------------------------------------

class PolygonCoordinates : protected std::vector<Point2> {
public:
    // -- Types
    using container_type = std::vector<Point2>;

    // -- Constructors

    PolygonCoordinates(const container_type& coordinates, bool removeAlignedPoints);

    PolygonCoordinates(const PolygonCoordinates&) = default;
    PolygonCoordinates(PolygonCoordinates&&)      = default;

    // -- Destructor

    virtual ~PolygonCoordinates() = default;

    // -- Operators

    PolygonCoordinates& operator=(const PolygonCoordinates&) = default;
    PolygonCoordinates& operator=(PolygonCoordinates&&)      = default;

    // -- Methods

    const Point2& max() const { return max_; }
    const Point2& min() const { return min_; }

    using container_type::operator[];
    using container_type::size;

protected:
    // -- Methods

    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const PolygonCoordinates&);

    // -- Members

    Point2 max_;
    Point2 min_;
};

//----------------------------------------------------------------------------------------------------------------------

/// @brief Implement PolygonCoordinates::contains for a polygon defined in lon/lat space.
class LonLatPolygon : public PolygonCoordinates {
public:
    explicit LonLatPolygon(const std::vector<Point2>&, bool removeAlignedPoints);
    explicit LonLatPolygon(const std::vector<Point2>&);

    /// @brief Point-in-polygon test based on winding number
    /// @note reference <a href="http://geomalgorithms.com/a03-_inclusion.html">Inclusion of a Point in a Polygon</a>
    /// @param[in] P given point
    /// @return if point (lon,lat) is in polygon
    bool contains(const Point2& Plonlat) const;

private:
    const bool includeNorthPole_;
    const bool includeSouthPole_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace polygon
}  // namespace geometry
}  // namespace eckit
