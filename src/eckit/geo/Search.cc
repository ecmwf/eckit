// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Search.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <map>
#include <ostream>

#include "eckit/eckit.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Trace.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/projection/LonLatHToXYZ.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/log/Log.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo {


namespace {


using KDTree = search::KDTree;


std::string points_uid(const std::vector<PointXYZ>& points) {
    MD5 hash;
    hash.add(points.size());
    for (const auto& p : points) {
        hash.add(p.X());
        hash.add(p.Y());
        hash.add(p.Z());
    }
    return hash.digest();
}


void sort(search::Neighbours& v) {
    std::sort(v.begin(), v.end(), [](const auto& a, const auto& b) {
        return a.distance < b.distance || (a.distance == b.distance && a.index < b.index);
    });
}


void check_radius(double radius) {
    if (!(radius >= 0.)) {
        throw exception::SearchError("Search: invalid radius " + std::to_string(radius), Here());
    }
}


bool has_heights(const Grid& grid) {
    return !grid.empty() && std::holds_alternative<PointLonLatH>(grid.first_point());
}


std::unique_ptr<const projection::LonLatHToXYZ> llh_to_xyz(const Figure& figure, const Search::Spec& spec,
                                                           bool heights) {
    if (!heights && !spec.has("height-scale-factor")) {
        return nullptr;
    }

    if (!heights) {
        throw exception::SearchError("Search: 'height-scale-factor' requires points with heights", Here());
    }

    return std::make_unique<projection::LonLatHToXYZ>(FigureFactory::build(figure),
                                                      spec.get_double("height-scale-factor", 1.));
}


// static schedule, so each thread queries a contiguous range (of nearby points, in query order); exceptions can't leave
// an OpenMP region, the first is rethrown after
template <typename F>
void parallel_for(size_t n, const F& f) {
#if eckit_HAVE_OMP
    std::exception_ptr error;
    const auto num_threads = into_signed(LibEcKitGeo::searchNumThreads());

#pragma omp parallel for schedule(static) num_threads(num_threads)
    for (long i = 0; i < into_signed(n); ++i) {
        try {
            f(into_unsigned(i));
        }
        catch (...) {
#pragma omp critical(eckit_geo_search_error)
            {
                if (!error) {
                    error = std::current_exception();
                }
            }
        }
    }

    if (error) {
        std::rethrow_exception(error);
    }
#else
    for (size_t i = 0; i < n; ++i) {
        f(i);
    }
#endif
}


void check_int(size_t value, const std::string& what) {
    if (value > into_unsigned(std::numeric_limits<int>::max())) {
        throw exception::SearchError("Search::CSR: " + what + " " + std::to_string(value) + " > INT_MAX", Here());
    }
}


}  // namespace


struct Search::Shared {
    std::unique_ptr<KDTree> kdtree;
    std::unique_ptr<const projection::LonLatToXYZ> to_xyz;
    std::unique_ptr<const projection::LonLatHToXYZ> llh_to_xyz;  // if points have heights
};


const Search::Storage& Search::Storage::library() {
    static const Storage storage{LibEcKitGeo::searchCacheLoader(), cache::SearchCache::file_cache()};
    return storage;
}


void Search::build(const std::string& uid, size_t size, const Spec& spec, const Storage& storage,
                   const KDTree::Fill& fill) {
    auto& kdtree = shared_->kdtree;
    kdtree       = search::KDTreeFactory::build(storage.loader, uid, size, storage.files);

    Trace trace("Search: build or load " + uid);
    if (!kdtree->build(fill, spec.get_bool("search-fast-build", LibEcKitGeo::searchFastBuild()))) {
        Log::debug() << "Search: loaded " << *kdtree << std::endl;
    }
}


std::string Search::uid(const Grid& grid, const Spec& spec) {
    const auto& figure = grid.figure();
    const auto llh     = llh_to_xyz(figure, spec, has_heights(grid));
    return MD5{grid.uid() + (llh ? llh->spec_str() : projection::LonLatToXYZ(FigureFactory::build(figure)).spec_str())}
        .digest();
}


Search::Search(const Grid& grid, const Spec& spec, const Storage& storage) : shared_(std::make_shared<Shared>()) {
    configure(spec);

    // (lon, lat) queries are on the figure of the grid points, as Grid::to_xyz
    const auto& figure  = grid.figure();
    shared_->to_xyz     = std::make_unique<projection::LonLatToXYZ>(FigureFactory::build(figure));
    shared_->llh_to_xyz = llh_to_xyz(figure, spec, has_heights(grid));

    // points off the figure (e.g. of space_view grids, beyond the visible disc) aren't searchable, so the k-d tree has
    // the other points only (of the grid indices), and its size is known before it is built or loaded
    std::vector<KDTree::Value> values;
    values.reserve(grid.size());

    auto add = [&values](const PointXYZ& p, size_t i) {
        if (std::isfinite(p.X()) && std::isfinite(p.Y()) && std::isfinite(p.Z())) {
            values.emplace_back(p, i);
        }
    };

    if (const auto* llh = shared_->llh_to_xyz.get(); llh != nullptr) {
        size_t i = 0;
        for (const auto& p : grid) {
            add(llh->fwd(std::get<PointLonLatH>(p)), i++);
        }
    }
    else {
        const auto xyz = grid.to_xyz();
        ASSERT(xyz.size() == 3);

        for (size_t i = 0; i < xyz[0].size(); ++i) {
            add(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, i);
        }
    }

    build(uid(grid, spec), values.size(), spec, storage, [&values](auto& v) { v = std::move(values); });
}


Search::Search(const std::vector<PointXYZ>& points, const Spec& spec, const Storage& storage) :
    shared_(std::make_shared<Shared>()) {
    configure(spec);

    shared_->to_xyz     = std::make_unique<projection::LonLatToXYZ>(spec);
    shared_->llh_to_xyz = llh_to_xyz(shared_->to_xyz->figure(), spec, false);

    build(points_uid(points), points.size(), spec, storage, [&points](std::vector<KDTree::Value>& values) {
        values.reserve(points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            values.emplace_back(points[i], i);
        }
    });
}


Search::Search(const Search& other, const Spec& spec) : shared_(other.shared_) {
    configure(spec);
}


Search::~Search() = default;


size_t Search::size() const {
    return shared_->kdtree->size();
}


Search::MemoryUsage Search::footprint() const {
    return shared_->kdtree->footprint();
}


const search::KDTree& Search::kdtree() const {
    return *shared_->kdtree;
}


Search::MemoryUsage Search::estimate(size_t size, const Storage& storage) {
    return search::KDTreeFactory::estimate(storage.loader, size);
}


std::unique_ptr<spec::Custom> Search::spec_kdtree(const Spec& spec) {
    auto kdtree = std::make_unique<spec::Custom>();

    if (spec.has("search-fast-build")) {
        kdtree->set("search-fast-build", spec.get_bool("search-fast-build"));
    }

    if (spec.has("height-scale-factor")) {
        kdtree->set("height-scale-factor", spec.get_double("height-scale-factor"));
    }

    return kdtree;
}


bool Search::spec_has_behaviour(const Spec& spec) {
    return spec.has("search") || spec.has("search-k") || spec.has("search-radius") || spec.has("search-query-order");
}


void Search::configure(const Spec& spec) {
    static const std::map<std::string, Mode> MODES{
        {"nn", Mode::NN},
        {"knn", Mode::KNN},
        {"radius", Mode::RADIUS},
        {"knn_or_radius", Mode::KNN_OR_RADIUS},
        {"knn_and_radius", Mode::KNN_AND_RADIUS},
        {"knn_or_same_radius", Mode::KNN_OR_SAME_RADIUS},
    };

    const auto name = spec.get_string("search", "nn");
    auto it         = MODES.find(name);
    if (it == MODES.end()) {
        throw exception::SearchError("Search: unknown search '" + name + "'", Here());
    }

    mode_ = it->second;

    if (mode_ == Mode::KNN || mode_ == Mode::KNN_OR_RADIUS || mode_ == Mode::KNN_AND_RADIUS ||
        mode_ == Mode::KNN_OR_SAME_RADIUS) {
        if (!spec.has("search-k")) {
            throw exception::SearchError("Search: search '" + name + "' requires 'search-k'", Here());
        }
        k_ = spec.get_unsigned("search-k");
    }

    if (mode_ == Mode::RADIUS || mode_ == Mode::KNN_OR_RADIUS || mode_ == Mode::KNN_AND_RADIUS) {
        if (!spec.has("search-radius")) {
            throw exception::SearchError("Search: search '" + name + "' requires 'search-radius'", Here());
        }
        radius_ = spec.get_double("search-radius");
        check_radius(radius_);
    }

    if (mode_ == Mode::KNN_OR_SAME_RADIUS) {
        tolerance_ = spec.get_double("search-tolerance", tolerance_);
        check_radius(tolerance_);
    }

    if (const auto order = spec.get_string("search-query-order", "morton"); order == "input") {
        query_order_ = QueryOrder::INPUT;
    }
    else if (order != "morton") {
        throw exception::SearchError("Search: unknown search-query-order '" + order + "' (morton or input)", Here());
    }
}


Search::Neighbours Search::search(const Point& p) const {
    switch (mode_) {
        case Mode::NN:
            return {search_nn(p)};
        case Mode::KNN:
            return search_knn(p, k_);
        case Mode::RADIUS:
            return search_radius(p, radius_);
        case Mode::KNN_OR_RADIUS:
            return search_knn_or_radius(p, k_, radius_);
        case Mode::KNN_AND_RADIUS:
            return search_knn_and_radius(p, k_, radius_);
        case Mode::KNN_OR_SAME_RADIUS:
            return search_knn_or_same_radius(p, k_, tolerance_);
    }

    NOTIMP;
}


std::vector<Search::Neighbours> Search::search(const std::vector<Point>& points) const {
    return search_xyz(to_xyz(points));
}


std::vector<Search::Neighbours> Search::search_xyz(const std::vector<PointXYZ>& xyz) const {
    std::vector<Neighbours> result(xyz.size());

    if (query_order_ == QueryOrder::INPUT) {
        parallel_for(xyz.size(), [this, &xyz, &result](size_t i) { result[i] = search(xyz[i]); });
        return result;
    }

    const auto order = KDTree::query_order(xyz);
    parallel_for(order.size(), [this, &order, &xyz, &result](size_t j) {
        const auto i = order[j];
        result[i]    = search(xyz[i]);
    });

    return result;
}


Search::CSR Search::csr_search(const std::vector<PointXYZ>& points, ColumnOrder order) const {
    check_int(size(), "points");

    auto neighbours = search_xyz(points);

    CSR csr;
    csr.cols = size();

    // offsets by rows of neighbours, then the rows copied (and released) on threads
    csr.offsets.resize(points.size() + 1);
    size_t entries = 0;
    for (size_t i = 0; i < neighbours.size(); ++i) {
        entries += neighbours[i].size();
        check_int(entries, "neighbours");
        csr.offsets[i + 1] = static_cast<int>(entries);
    }

    csr.indices.resize(entries);
    csr.distances.resize(entries);
    parallel_for(neighbours.size(), [&csr, &neighbours, order](size_t i) {
        auto& n = neighbours[i];
        if (order == ColumnOrder::INDEX) {
            std::sort(n.begin(), n.end(), [](const auto& a, const auto& b) { return a.index < b.index; });
        }

        auto j = into_unsigned(csr.offsets[i]);
        for (const auto& m : n) {
            csr.indices[j]     = static_cast<int>(m.index);
            csr.distances[j++] = m.distance;
        }

        Neighbours().swap(n);
    });

    return csr;
}


Search::CSR Search::csr_search(const std::vector<PointLonLat>& points, ColumnOrder order) const {
    return csr_search(to_xyz(points), order);
}


Search::CSR Search::csr_search(const std::vector<PointLonLatH>& points, ColumnOrder order) const {
    return csr_search(to_xyz(points), order);
}


Search::Neighbour Search::search_nn(const Point& p) const {
    return shared_->kdtree->nearest_neighbour(to_xyz(p));
}


Search::Neighbours Search::search_knn(const Point& p, size_t k) const {
    if (k == 0) {
        return {};
    }

    if (k == 1) {
        return {search_nn(p)};
    }

    auto result = shared_->kdtree->k_nearest_neighbours(to_xyz(p), k);
    sort(result);
    return result;
}


Search::Neighbours Search::search_radius(const Point& p, double radius) const {
    check_radius(radius);

    auto result = shared_->kdtree->find_in_sphere(to_xyz(p), radius);
    sort(result);
    return result;
}


Search::Neighbours Search::search_knn_or_radius(const Point& p, size_t k, double radius) const {
    auto result = search_radius(p, radius);
    auto knn    = search_knn(p, k);

    result.insert(result.end(), knn.begin(), knn.end());

    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.index < b.index; });
    result.erase(
        std::unique(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.index == b.index; }),
        result.end());

    sort(result);
    return result;
}


Search::Neighbours Search::search_knn_and_radius(const Point& p, size_t k, double radius) const {
    check_radius(radius);

    auto result = search_knn(p, k);
    result.erase(std::remove_if(result.begin(), result.end(), [radius](const auto& n) { return n.distance > radius; }),
                 result.end());

    return result;
}


Search::Neighbours Search::search_knn_or_same_radius(const Point& p, size_t k, double tolerance) const {
    check_radius(tolerance);

    auto knn = search_knn(p, k);
    if (knn.empty() || !types::is_approximately_equal(knn.back().distance, knn.front().distance, tolerance)) {
        return knn;
    }

    return search_radius(p, knn.back().distance + tolerance);
}


template <typename P>
std::vector<PointXYZ> Search::to_xyz(const std::vector<P>& points) const {
    std::vector<PointXYZ> xyz(points.size());
    parallel_for(points.size(), [this, &points, &xyz](size_t i) { xyz[i] = to_xyz(Point{points[i]}); });
    return xyz;
}


PointXYZ Search::to_xyz(const Point& p) const {
    if (std::holds_alternative<PointXYZ>(p)) {
        return std::get<PointXYZ>(p);
    }

    // on points with heights, (lon, lat) would be at height 0, and find wrong neighbours
    if (const auto* q = std::get_if<PointLonLat>(&p); q != nullptr) {
        if (shared_->llh_to_xyz) {
            throw exception::SearchError(
                "Search: (lon, lat) queries of points with heights, expected (lon, lat, height)", Here());
        }
        return shared_->to_xyz->fwd(*q);
    }

    if (const auto* q = std::get_if<PointLonLatH>(&p); q != nullptr) {
        if (!shared_->llh_to_xyz) {
            throw exception::SearchError(
                "Search: (lon, lat, height) queries require points with heights (see height-scale-factor)", Here());
        }
        return shared_->llh_to_xyz->fwd(*q);
    }

    throw exception::SearchError("Search: unsupported point type (supported: PointXYZ, PointLonLat, PointLonLatH)",
                                 Here());
}


void Search::print(std::ostream& out) const {
    out << "Search[kdtree=" << *shared_->kdtree << "]";
}


}  // namespace eckit::geo
