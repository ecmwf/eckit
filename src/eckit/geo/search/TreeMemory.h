// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/container/KDTree.h"
#include "eckit/geo/search/Tree.h"


namespace eckit::geo::search {


class TreeMemory : public Tree {
protected:

    KDTreeMemory<Tree> tree_;

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


}  // namespace eckit::geo::search
