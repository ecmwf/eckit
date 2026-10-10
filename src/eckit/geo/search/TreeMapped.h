// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/container/KDTree.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/search/Tree.h"


namespace eckit::geo::search {


class TreeMapped : public Tree {
public:

    // -- Types

    using KDTree = KDTreeMapped<Traits>;

    // -- Overridden methods

    void build(std::vector<Value>&) override;
    void insert(const Value&) override;

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

    /// Mapped storage is private to the process
    MemoryUsage footprint() const override { return {is_open() ? storage_size() : 0, 0}; }

    size_t storage_size() const;

protected:

    // -- Constructors

    using Tree::Tree;

    // -- Methods

    /// Created (for size() points), or existing
    void open(const PathName&, bool create);
    void close();
    bool is_open() const { return static_cast<bool>(tree_); }

    /// Complete, and of the expected layout
    bool valid(const PathName&) const;

private:

    // -- Members

    std::unique_ptr<KDTree> tree_;
    size_t count_ = 0;

    // -- Methods

    KDTree& tree();
    const KDTree& tree() const;
};


}  // namespace eckit::geo::search
