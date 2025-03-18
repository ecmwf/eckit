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

#include <deque>
#include <iosfwd>

#include "eckit/geo/Point2.h"


namespace eckit::geo::polygon {


class Polygon2 : public std::deque<Point2> {
public:

    // -- Types

    using container_type = deque;
    using container_type::value_type;

    // -- Constructors

    using container_type::container_type;

    // -- Operators

    bool operator==(const Polygon2&) const;
    bool operator!=(const Polygon2& other) const { return !(*this == other); }

    // -- Methods

    /**
     * @brief Point-in-polygon test based on winding number
     * @param[in] P given point
     * @return if point is in polygon
     */
    bool contains(const Point2& P) const;

private:

    // -- Methods

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Polygon2& p) {
        p.print(s);
        return s;
    }
};


}  // namespace eckit::geo::polygon
