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

#include "mir/search/Tree.h"


namespace mir::param {
class MIRParametrisation;
}  // namespace mir::param


namespace mir::search {


/// Class for fast searches in point clouds following k-d tree algorithms
/// @todo test k-d tree stored in shared memory?
class PointSearch {
public:
    // -- Types

    using ValueType      = Tree::Payload;
    using PointType      = Tree::Point;
    using PointValueType = Tree::PointValueType;

    // -- Exceptions
    // None

    // -- Constructors

    PointSearch(const param::MIRParametrisation&, const repres::Representation&);

    PointSearch(const PointSearch&) = delete;
    PointSearch(PointSearch&&)      = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    void operator=(const PointSearch&) = delete;
    void operator=(PointSearch&&)      = delete;

    // -- Methods

    /// Finds closest point to an input point
    PointValueType closestPoint(const PointType&) const;

    /// Finds closest N points to an input point
    void closestNPoints(const PointType&, size_t n, std::vector<PointValueType>& closest) const;

    /// Finds closest points within a radius
    void closestWithinRadius(const PointType&, double radius, std::vector<PointValueType>& closest) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::unique_ptr<Tree> tree_;

    // -- Methods

    void build(const repres::Representation&);

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointSearch& p) {
        p.print(out);
        return out;
    }
};


}  // namespace mir::search
