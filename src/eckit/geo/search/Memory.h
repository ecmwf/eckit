// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/container/KDTree.h"
#include "eckit/geo/search/KDTree.h"


namespace eckit::geo::search {


class Memory final : public KDTree {
public:

    // -- Types

    using Container = KDTreeMemory<Traits>;

    // -- Constructors

    Memory(const std::string& uid, size_t size);

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {size * sizeof(Container::Node), 0}; }

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

    MemoryUsage footprint() const override { return {count_ * sizeof(Container::Node), 0}; }

private:

    // -- Members

    Container tree_;
    size_t count_ = 0;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
