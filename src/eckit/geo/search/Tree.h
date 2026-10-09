// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/container/sptree/SPValue.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/geo/cache/MemoryUsage.h"
#include "eckit/memory/Builder.h"


namespace eckit::geo::search {


struct Traits {
    using Point   = PointXYZ;
    using Payload = size_t;
};


struct Neighbour {
    PointXYZ point;
    size_t index;
    double distance;

    friend bool operator==(const Neighbour& a, const Neighbour& b) {
        return a.index == b.index && a.distance == b.distance && a.point == b.point;
    }

    friend bool operator!=(const Neighbour& a, const Neighbour& b) { return !(a == b); }

    friend std::ostream& operator<<(std::ostream&, const Neighbour&);
};


using Neighbours = std::vector<Neighbour>;


/**
 * @brief k-d tree, abstracting its storage
 * @details Building is: lock(), if not ready() then build() (or insert()) and commit(), unlock(). Queries update
 * statistics, so they need external synchronisation.
 */
class Tree {
public:

    // -- Types

    using Point       = Traits::Point;
    using Payload     = Traits::Payload;
    using Value       = SPValue<Traits>;
    using MemoryUsage = cache::MemoryUsage;

    using builder_t = BuilderT2<Tree>;
    using ARG1      = const std::string&;
    using ARG2      = size_t;

    // -- Constructors

    Tree(const std::string& uid, size_t size);

    Tree(const Tree&) = delete;
    Tree(Tree&&)      = delete;

    // -- Destructor

    virtual ~Tree();

    // -- Operators

    Tree& operator=(const Tree&) = delete;
    Tree& operator=(Tree&&)      = delete;

    // -- Methods

    /// Balanced (container is reordered)
    virtual void build(std::vector<Value>&) = 0;

    /// Unbalanced
    virtual void insert(const Value&) = 0;

    virtual Neighbour nearest_neighbour(const Point&)               = 0;
    virtual Neighbours k_nearest_neighbours(const Point&, size_t k) = 0;
    virtual Neighbours find_in_sphere(const Point&, double radius)  = 0;

    /// If built already (e.g. cached)
    virtual bool ready() = 0;

    /// Finalise building (e.g. persist)
    virtual void commit() = 0;

    /// Exclusive building, across threads and processes as applicable (BasicLockable)
    virtual void lock() {}
    virtual void unlock() {}

    virtual void stats_print(std::ostream&, bool pretty) const = 0;
    virtual void stats_reset()                                 = 0;

    virtual MemoryUsage footprint() const = 0;

    const std::string& uid() const { return uid_; }
    size_t size() const { return size_; }

    // -- Class methods

    static std::string className() { return "tree"; }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Tree& tree) {
        tree.print(out);
        return out;
    }

protected:

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    template <typename NodeInfo>
    static Neighbour to_neighbour(const NodeInfo& n) {
        return {n.point(), n.payload(), n.distance()};
    }

    template <typename NodeList>
    static Neighbours to_neighbours(const NodeList& list) {
        Neighbours result;
        result.reserve(list.size());
        for (const auto& n : list) {
            result.emplace_back(to_neighbour(n));
        }
        return result;
    }

private:

    // -- Members

    const std::string uid_;
    const size_t size_;
};


template <typename T>
using TreeRegisterType = ConcreteBuilderT2<Tree, T>;


struct TreeFactory {
    [[nodiscard]] static Tree* build(const std::string& type, const std::string& uid, size_t size);
    static bool has_type(const std::string& type) { return Factory<Tree>::instance().exists(type); }
    static std::ostream& list(std::ostream&);
};


}  // namespace eckit::geo::search
