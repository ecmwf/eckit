/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_search_Search_h
#define mir_search_Search_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"

#include "mir/search/Tree.h"


namespace mir {
namespace search {


/// Class for fast searches in point clouds following k-d tree algorithms
/// @todo test kd-tree stored in shared memory?
class PointSearch : private eckit::NonCopyable {
public:

    using ValueType = Tree::Payload;
    using PointType = Tree::Point;
    using PointValueType = Tree::PointValueType;

public:

    PointSearch(const param::MIRParametrisation& parametrisation,
           const repres::Representation&);

public:

    /// Finds closest point to an input point
    PointValueType closestPoint(const PointType&) const;

    /// Finds closest N points to an input point
    void closestNPoints(const PointType&, size_t n, std::vector<PointValueType>& closest) const;

    /// Finds closest points within a radius
    void closestWithinRadius(const PointType&, double radius, std::vector<PointValueType>& closest) const;

    void statsPrint(std::ostream&, bool fancy) const;
    void statsReset() const;

private:

    const param::MIRParametrisation& parametrisation_;
    eckit::ScopedPtr<Tree> tree_;

    void build(const repres::Representation& r);
};


}  // namespace search
}  // namespace mir


#endif

