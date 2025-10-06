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

#include "eckit/geometry/Point2.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry::polygon {

//------------------------------------------------------------------------------------------------------

class Polygon : protected std::deque<Point2> {
public:

    using container_type = std::deque<value_type>;
    using container_type::value_type;

    Polygon() = default;

    Polygon(std::initializer_list<value_type> l) : container_type(l) {}

    using container_type::push_back;
    using container_type::push_front;

    size_t num_vertices() const { return size(); }

    const value_type& vertex(size_t idx) const { return at(idx); }

    bool sameAs(const Polygon& p) const { return *this == p; }

    bool congruent(const Polygon&) const;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Polygon& p) {
        p.print(s);
        return s;
    }
};

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry::polygon
