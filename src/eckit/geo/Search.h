// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/search/Tree.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo {
class Grid;
namespace cache {
class SearchCache;
}
}  // namespace eckit::geo


namespace eckit::geo {


/**
 * @brief Search in point clouds, following k-d tree algorithms
 *
 * @details Points are geocentric (X, Y, Z): grid points as Grid::to_xyz, and queries either (X, Y, Z) or (lon, lat) on
 * the same figure. Distances are chord lengths (metres, on the Earth). Results are by increasing distance, then index.
 *
 * Spec, of the behaviour: search (nn, knn, radius, knn_or_radius, knn_and_radius), search-k, search-radius.
 * Spec, of the k-d tree: search-tree (see search::TreeFactory), caching, search-fast-build (defaults in LibEcKitGeo).
 */
class Search {
public:

    // -- Types

    using Neighbour   = geo::search::Neighbour;
    using Neighbours  = geo::search::Neighbours;
    using MemoryUsage = cache::MemoryUsage;
    using Spec        = spec::Spec;

    // -- Constructors

    explicit Search(const Grid&, const Spec& = spec::Custom{});
    explicit Search(const std::vector<PointXYZ>&, const Spec& = spec::Custom{});

    /// Sharing the k-d tree of another search, with another behaviour
    Search(const Search&, const Spec&);

    Search(const Search&) = delete;
    Search(Search&&)      = delete;

    // -- Destructor

    ~Search();

    // -- Operators

    Search& operator=(const Search&) = delete;
    Search& operator=(Search&&)      = delete;

    // -- Methods

    /// As configured (spec)
    Neighbours search(const Point&) const;

    Neighbour search_nn(const Point&) const;
    Neighbours search_knn(const Point&, size_t k) const;
    Neighbours search_radius(const Point&, double radius) const;
    Neighbours search_knn_or_radius(const Point&, size_t k, double radius) const;
    Neighbours search_knn_and_radius(const Point&, size_t k, double radius) const;

    size_t size() const;

    MemoryUsage footprint() const;

    const geo::search::Tree& tree() const;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Search& s) {
        s.print(out);
        return out;
    }

    friend class cache::SearchCache;

private:

    // -- Types

    enum class Mode {
        NN,
        KNN,
        RADIUS,
        KNN_OR_RADIUS,
        KNN_AND_RADIUS,
    };

    /// k-d tree and (lon, lat) conversion, shareable between searches
    struct Shared;

    // -- Members

    std::shared_ptr<Shared> shared_;

    Mode mode_     = Mode::NN;
    size_t k_      = 0;
    double radius_ = 0.;

    // -- Methods

    void configure(const Spec&);

    template <typename Fill>
    void build(const std::string& uid, size_t size, const Spec&, const Fill&);

    PointXYZ to_xyz(const Point&) const;

    void print(std::ostream&) const;

    // -- Class methods

    /// k-d tree configuration, so searches of other behaviours can share it
    static std::unique_ptr<spec::Custom> spec_tree(const Spec&);
    static bool spec_has_behaviour(const Spec&);
};


}  // namespace eckit::geo
