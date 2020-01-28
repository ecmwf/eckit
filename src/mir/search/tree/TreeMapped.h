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


#ifndef mir_search_tree_TreeMapped_h
#define mir_search_tree_TreeMapped_h

#include "eckit/container/KDTree.h"
#include "eckit/os/AutoUmask.h"
#include "mir/search/Tree.h"


namespace mir {
namespace search {
namespace tree {


class TreeMapped : public Tree {

protected:
    eckit::AutoUmask umask_;  // Must be first
    eckit::PathName path_;
    eckit::KDTreeMapped<Tree> tree_;

    void build(std::vector<PointValueType>&);

    void insert(const PointValueType&);

    void statsPrint(std::ostream&, bool pretty);

    void statsReset();

    PointValueType nearestNeighbour(const Tree::Point&);

    std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k);

    std::vector<PointValueType> findInSphere(const Point&, double radius);

    virtual bool ready() const = 0;

    virtual void commit() = 0;

    virtual void print(std::ostream& out) const = 0;

public:
    TreeMapped(const repres::Representation&, const eckit::PathName&);
};


}  // namespace tree
}  // namespace search
}  // namespace mir


#endif
