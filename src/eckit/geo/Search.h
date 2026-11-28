// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Point.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/spec/Custom.h"


namespace eckit::cache {
class FileCache;
}
namespace eckit::geo {
class Grid;
}


namespace eckit::geo {


/**
 * @brief Search in point clouds, following k-d tree algorithms
 *
 * @details Points are geocentric (X, Y, Z): grid points as Grid::to_xyz, and queries either (X, Y, Z) or (lon, lat) on
 * the same figure. Distances are chord lengths (metres, on the Earth). Results are by increasing distance, then index.
 * Queries can run concurrently.
 *
 * Grid points with heights are in a space distorted by height-scale-factor s (see projection::LonLatHToXYZ): distances
 * are as sqrt(horizontal^2 + (s dh)^2), not metres unless s is 1. Their queries are (lon, lat, height), not (lon, lat).
 *
 * Spec, of the behaviour: search (nn, knn, radius, knn_or_radius, knn_and_radius, knn_or_same_radius), search-k,
 * search-radius, search-tolerance (metres, default 1), search-query-order of many points (morton, default, or input if
 * consecutive queries are already near each other, e.g. grid points).
 * Spec, of the k-d tree: search-fast-build (default LibEcKitGeo::searchFastBuild), height-scale-factor (default 1,
 * for points with heights); its storage (loader and cache files) is of the library (see Storage::library) unless given.
 */
class Search {
public:

    // -- Types

    using Neighbour   = geo::search::Neighbour;
    using Neighbours  = geo::search::Neighbours;
    using MemoryUsage = eckit::cache::MemoryUsage;
    using Spec        = spec::Spec;

    /// Where k-d trees are kept: in process memory, or in cache files loaded by a loader (see eckit::cache::Loader)
    struct Storage {
        std::string loader;
        const eckit::cache::FileCache& files;

        /// As the library: LibEcKitGeo::searchCacheLoader, in cache::SearchCache::file_cache
        static const Storage& library();
    };

    enum class ColumnOrder {
        DISTANCE,  // then index, as Neighbours
        INDEX,
    };

    // Sparse matrix of distances, row per query and column per point. int, not size_t, as eckit::linalg::SparseMatrix
    // Index and scipy.sparse int32 indptr/indices, so the arrays back either without copying (beyond INT_MAX throws)
    struct CSR {
        size_t rows() const { return offsets.size() - 1; }
        size_t size() const { return indices.size(); }

        size_t cols = 0;
        std::vector<int> offsets{0};
        std::vector<int> indices;
        std::vector<double> distances;
    };

    // -- Constructors

    explicit Search(const Grid&, const Spec& = spec::Custom{}, const Storage& = Storage::library());
    explicit Search(const std::vector<PointXYZ>&, const Spec& = spec::Custom{}, const Storage& = Storage::library());

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

    // Search method as configured (spec)
    Neighbours search(const Point&) const;
    std::vector<Neighbours> search(const std::vector<Point>&) const;

    // CSR multi-query search
    CSR csr_search(const std::vector<PointXYZ>&, ColumnOrder = ColumnOrder::DISTANCE) const;
    CSR csr_search(const std::vector<PointLonLat>&, ColumnOrder = ColumnOrder::DISTANCE) const;
    CSR csr_search(const std::vector<PointLonLatH>&, ColumnOrder = ColumnOrder::DISTANCE) const;

    // Search individual methods
    Neighbour search_nn(const Point&) const;
    Neighbours search_knn(const Point&, size_t k) const;
    Neighbours search_radius(const Point&, double radius) const;
    Neighbours search_knn_or_radius(const Point&, size_t k, double radius) const;
    Neighbours search_knn_and_radius(const Point&, size_t k, double radius) const;
    Neighbours search_knn_or_same_radius(const Point&, size_t k, double tolerance = 1.) const;

    size_t size() const;

    MemoryUsage footprint() const;

    const geo::search::KDTree& kdtree() const;

    // -- Class methods

    /// k-d tree identifier of a grid: its points, on the figure they are converted on (see Grid::to_xyz)
    static std::string uid(const Grid&, const Spec& = spec::Custom{});

    /// k-d tree configuration, so searches of other behaviours can share it
    static std::unique_ptr<spec::Custom> spec_kdtree(const Spec&);

    /// Footprint of a search of size points, before building or loading it
    static MemoryUsage estimate(size_t size, const Storage& = Storage::library());

    /// If the spec sets a behaviour, so the search is not the shared one as is
    static bool spec_has_behaviour(const Spec&);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const Search& s) {
        s.print(out);
        return out;
    }

private:

    // -- Types

    enum class QueryOrder {
        MORTON,
        INPUT,
    };

    enum class Mode {
        NN,
        KNN,
        RADIUS,
        KNN_OR_RADIUS,
        KNN_AND_RADIUS,
        KNN_OR_SAME_RADIUS,
    };

    /// k-d tree and (lon, lat) conversion, shareable between searches
    struct Shared;

    // -- Members

    std::shared_ptr<Shared> shared_;

    Mode mode_        = Mode::NN;
    size_t k_         = 0;
    double radius_    = 0.;
    double tolerance_ = 1.;

    QueryOrder query_order_ = QueryOrder::MORTON;

    // -- Methods

    void configure(const Spec&);

    void build(const std::string& uid, size_t size, const Spec&, const Storage&, const search::KDTree::Fill&);

    PointXYZ to_xyz(const Point&) const;

    template <typename P>
    std::vector<PointXYZ> to_xyz(const std::vector<P>&) const;

    std::vector<Neighbours> search_xyz(const std::vector<PointXYZ>&) const;

    void print(std::ostream&) const;
};


}  // namespace eckit::geo
