// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/container/KDTree.h"
#include "eckit/geo/search/Tree.h"


namespace eckit::geo::search {


class TreeMemory final : public Tree {
public:

    // -- Types

    using KDTree = KDTreeMemory<Traits>;

    // -- Constructors

    TreeMemory(const std::string& uid, size_t size);

    // -- Overridden methods

    void build(std::vector<Value>&) override;
    void insert(const Value&) override;

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    bool ready() override { return false; }
    void commit() override {}

    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

    MemoryUsage footprint() const override { return {count_ * sizeof(KDTree::Node), 0}; }

private:

    // -- Members

    KDTree tree_;
    size_t count_ = 0;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
