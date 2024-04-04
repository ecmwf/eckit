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

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geo::polygon {

//----------------------------------------------------------------------------------------------------------------------

class LonLatPolygon : protected std::vector<PointLonLat> {
public:
    // -- Types
    using container_type = std::vector<PointLonLat>;
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

    /// @brief Point-in-polygon test based on winding number
    /// @ref http://geomalgorithms.com/a03-_inclusion.html
    /// @param[in] P given point
    /// @return if point is in polygon
    bool contains(const PointLonLat& P) const;

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

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geo::polygon
