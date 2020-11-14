/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Polygon_H
#define Polygon_H

#include <deque>
#include "eckit/geometry/Point2.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

class Polygon {
public:
    Polygon() {}

    Polygon(std::initializer_list<Point2> vv) {
        for (auto& v : vv)
            vertices.push_back(v);
    }

    void push_back(const Point2& p) { vertices.push_back(p); }
    void push_front(const Point2& p) { vertices.push_front(p); }
    size_t num_vertices() const { return vertices.size(); }
    const Point2& vertex(size_t idx) const { return vertices.at(idx); }
    bool sameAs(const Polygon& p) const { return vertices == p.vertices; }
    bool congruent(const Polygon& p) const;


    void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const Polygon& p) {
        p.print(s);
        return s;
    }

private:  // members
    std::deque<Point2> vertices;
};

//------------------------------------------------------------------------------------------------------

}  // namespace geometry
}  // namespace eckit

#endif  // Polygon_H
