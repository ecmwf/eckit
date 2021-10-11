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

#include "eckit/container/KDTree.h"

#include "mir/search/Tree.h"


namespace mir {
namespace search {
namespace tree {


class TreeMemory : public Tree {

protected:
    eckit::KDTreeMemory<Tree> tree_;

    void build(std::vector<PointValueType>&) override;

    void insert(const PointValueType&) override;

    void statsPrint(std::ostream&, bool pretty) override;

    void statsReset() override;

    PointValueType nearestNeighbour(const Tree::Point&) override;

    std::vector<PointValueType> kNearestNeighbours(const Point&, size_t k) override;

    std::vector<PointValueType> findInSphere(const Point&, double radius) override;

    bool ready() const override;

    void commit() override;

    void print(std::ostream&) const override;

public:
    using Tree::Tree;
};


}  // namespace tree
}  // namespace search
}  // namespace mir
