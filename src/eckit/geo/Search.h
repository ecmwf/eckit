/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <memory>

#include "eckit/geo/search/Tree.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo {
class Grid;
}


namespace eckit::geo {


class Search {
public:

    // -- Types

    using ValueType      = search::Tree::Payload;
    using PointType      = search::Tree::Point;
    using PointValueType = search::Tree::PointValueType;

    // -- Constructors

    explicit Search(const Grid&, const Spec& = spec::Custom{});

    Search(const Search&) = delete;
    Search(Search&&)      = delete;

    // -- Operators

    void operator=(const Search&) = delete;
    void operator=(Search&&)      = delete;

    // -- Methods

    /// Finds closest point to an input point
    PointValueType closestPoint(const PointType&) const;

    /// Finds closest N points to an input point
    void closestNPoints(const PointType&, size_t n, std::vector<PointValueType>& closest) const;

    /// Finds closest points within a radius
    void closestWithinRadius(const PointType&, double radius, std::vector<PointValueType>& closest) const;

private:

    // -- Members

    std::unique_ptr<search::Tree> tree_;

    // -- Methods

    void build(const Grid&);

    void print(std::ostream&) const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Search& p) {
        p.print(out);
        return out;
    }
};


}  // namespace eckit::geo
