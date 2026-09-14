// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
