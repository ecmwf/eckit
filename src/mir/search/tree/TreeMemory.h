/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @author Peter Bispham
/// @date Apr 2015


#ifndef mir_search_tree_TreeMemory_h
#define mir_search_tree_TreeMemory_h

#include "eckit/container/KDTree.h"
#include "mir/search/Tree.h"


namespace mir {
namespace search {
namespace tree {


class TreeMemory : public Tree {

protected:
    eckit::KDTreeMemory<Tree> tree_;

    void build(std::vector<PointValueType>&);

    void insert(const PointValueType&);

    void statsPrint(std::ostream&, bool pretty);

    void statsReset();

    PointValueType nearestNeighbour(const Tree::Point&);

    std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k);

    std::vector<PointValueType> findInSphere(const Point&, double radius);

    virtual bool ready() const;

    virtual void commit();

    virtual void print(std::ostream& out) const;

public:
    TreeMemory(const repres::Representation&, const param::MIRParametrisation&);
};


} // namespace tree
} // namespace search
} // namespace mir


#endif
