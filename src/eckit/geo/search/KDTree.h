// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/cache/MemoryUsage.h"
#include "eckit/container/sptree/SPValue.h"
#include "eckit/geo/PointXYZ.h"


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
 * @brief k-d tree of any storage (process memory, file, shared memory)
 * @details Built once (see build), then queried; queries update statistics, so they need external synchronisation.
 */
class KDTree {
public:

    // -- Types

    using Point       = Traits::Point;
    using Payload     = Traits::Payload;
    using Value       = SPValue<Traits>;
    using MemoryUsage = eckit::cache::MemoryUsage;

    /// Fills exactly size() values (points and their index) to build from
    using Fill = std::function<void(std::vector<Value>&)>;

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

    /// Build balanced (reordering values) or by insertion, unless already built (e.g. cached); @return if built
    virtual bool build(const Fill&, bool balanced = true) = 0;

    virtual Neighbour nearest_neighbour(const Point&)               = 0;
    virtual Neighbours k_nearest_neighbours(const Point&, size_t k) = 0;
    virtual Neighbours find_in_sphere(const Point&, double radius)  = 0;

    virtual void stats_print(std::ostream&, bool pretty) const = 0;
    virtual void stats_reset()                                 = 0;

    virtual MemoryUsage footprint() const = 0;

    size_t size() const { return size_; }

    // -- Class methods

    /// Morton order to query many points in, so nearby points are queried consecutively (memory and page locality)
    static std::vector<size_t> query_order(const std::vector<Point>&);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const KDTree& kdtree) {
        kdtree.print(out);
        return out;
    }

protected:

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    /// Values to build from, checked against the size
    std::vector<Value> values(const Fill&) const;

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
 * @brief k-d trees by eckit::cache loader name (see LibEcKitGeo::searchCacheLoader)
 * @details "memory" builds in the process every time (see KDTreeMemory); other loaders use a cache file (see
 * KDTreeCache).
 */
class KDTreeFactory {
public:

    // -- Constructors

    KDTreeFactory() = delete;

    // -- Class methods

    [[nodiscard]] static KDTree* build(const std::string& loader, const std::string& uid, size_t size);

    /// Memory usage of a k-d tree of size points, once built or loaded
    static KDTree::MemoryUsage estimate(const std::string& loader, size_t size);

    /// If the loader is supported
    static bool has(const std::string& loader);
};


}  // namespace eckit::geo::search
