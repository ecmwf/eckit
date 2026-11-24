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
class KDTree {
public:

    // -- Types

    using Point       = Traits::Point;
    using Payload     = Traits::Payload;
    using Value       = SPValue<Traits>;
    using MemoryUsage = cache::MemoryUsage;

    // -- Constructors

    explicit KDTree(size_t size);

    KDTree(const KDTree&) = delete;
    KDTree(KDTree&&)      = delete;

    // -- Destructor

    virtual ~KDTree();

    // -- Operators

    KDTree& operator=(const KDTree&) = delete;
    KDTree& operator=(KDTree&&)      = delete;

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

    size_t size() const { return size_; }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const KDTree& kdtree) {
        kdtree.print(out);
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

    const size_t size_;
};


/**
 * @brief Loaders of k-d trees (spec search-loader), by name
 * @details Loaders (aliases): memory (no cache file, the default when not caching), mmap (mapped-memory, the default
 * when caching), file-io, shmem (shared-memory) and tmp-shmem (tmp-shared-memory). A loader is a KDTree type, providing
 * its memory usage before building or loading (T::estimate), so caches make room beforehand.
 */
class KDTreeFactory {
public:

    // -- Constructors

    KDTreeFactory(const KDTreeFactory&) = delete;
    KDTreeFactory(KDTreeFactory&&)      = delete;

    // -- Operators

    KDTreeFactory& operator=(const KDTreeFactory&) = delete;
    KDTreeFactory& operator=(KDTreeFactory&&)      = delete;

    // -- Class methods

    [[nodiscard]] static KDTree* build(const std::string& loader, const std::string& uid, size_t size);

    /// Memory usage of a k-d tree of size points, once built or loaded
    static KDTree::MemoryUsage estimate(const std::string& loader, size_t size);


protected:

    // -- Constructors

    explicit KDTreeFactory(const std::string& loader);

    // -- Destructor

    virtual ~KDTreeFactory();

private:

    // -- Members

    const std::string loader_;

    // -- Methods

    virtual KDTree* make(const std::string& uid, size_t size) const = 0;
    virtual KDTree::MemoryUsage make_estimate(size_t size) const    = 0;

    // -- Class methods

    static const KDTreeFactory& get(const std::string& loader);
    static std::ostream& list(std::ostream&);
};


template <typename T>
class KDTreeBuilder final : public KDTreeFactory {
public:

    explicit KDTreeBuilder(const std::string& loader) : KDTreeFactory(loader) {}

private:

    KDTree* make(const std::string& uid, size_t size) const override { return new T(uid, size); }
    KDTree::MemoryUsage make_estimate(size_t size) const override { return T::estimate(size); }
};


}  // namespace eckit::geo::search
