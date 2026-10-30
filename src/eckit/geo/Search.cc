// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Search.h"

#include <algorithm>
#include <map>
#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Trace.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"


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


}  // namespace


struct Search::Shared {
    std::unique_ptr<KDTree> kdtree;
    std::unique_ptr<const projection::LonLatToXYZ> to_xyz;

    // queries update the k-d tree statistics
    util::recursive_mutex mutex;
};


void Search::build(const std::string& uid, size_t size, const Spec& spec, const KDTree::Fill& fill) {
    auto& kdtree = shared_->kdtree;
    kdtree.reset(geo::search::KDTreeFactory::build(LibEcKitGeo::searchCacheLoader(), uid, size));

    Trace trace("Search: build or load " + uid);
    if (!kdtree->build(fill, spec.get_bool("search-fast-build", true))) {
        Log::debug() << "Search: loaded " << *kdtree << std::endl;
    }
}


std::string Search::uid(const Grid& grid) {
    const auto& figure = grid.figure();
    return MD5{grid.uid() + projection::LonLatToXYZ(figure.a(), figure.b()).spec_str()}.digest();
}


Search::Search(const Grid& grid, const Spec& spec) : shared_(std::make_shared<Shared>()) {
    configure(spec);

    // (lon, lat) queries are on the figure of the grid points, as Grid::to_xyz
    const auto& figure = grid.figure();
    shared_->to_xyz    = std::make_unique<projection::LonLatToXYZ>(figure.a(), figure.b());

    build(uid(grid), grid.size(), spec, [&grid](std::vector<KDTree::Value>& values) {
        const auto xyz = grid.to_xyz();
        ASSERT(xyz.size() == 3);

        values.reserve(xyz[0].size());
        for (size_t i = 0; i < xyz[0].size(); ++i) {
            values.emplace_back(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, i);
        }
    });
}


Search::Search(const std::vector<PointXYZ>& points, const Spec& spec) : shared_(std::make_shared<Shared>()) {
    configure(spec);

    shared_->to_xyz = std::make_unique<projection::LonLatToXYZ>(spec);

    build(points_uid(points), points.size(), spec, [&points](std::vector<KDTree::Value>& values) {
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


Search::MemoryUsage Search::estimate(size_t size) {
    return geo::search::KDTreeFactory::estimate(LibEcKitGeo::searchCacheLoader(), size);
}


std::unique_ptr<spec::Custom> Search::spec_kdtree(const Spec& spec) {
    auto kdtree = std::make_unique<spec::Custom>();

    if (spec.has("search-fast-build")) {
        kdtree->set("search-fast-build", spec.get_bool("search-fast-build"));
    }

    return kdtree;
}


bool Search::spec_has_behaviour(const Spec& spec) {
    return spec.has("search") || spec.has("search-k") || spec.has("search-radius");
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
    std::vector<PointXYZ> xyz;
    xyz.reserve(points.size());
    for (const auto& p : points) {
        xyz.emplace_back(to_xyz(p));
    }

    std::vector<Neighbours> result(points.size());

    // locked once for all queries
    util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
    for (auto i : KDTree::query_order(xyz)) {
        result[i] = search(xyz[i]);
    }

    return result;
}


Search::Neighbour Search::search_nn(const Point& p) const {
    const auto q = to_xyz(p);

    util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
    return shared_->kdtree->nearest_neighbour(q);
}


Search::Neighbours Search::search_knn(const Point& p, size_t k) const {
    if (k == 0) {
        return {};
    }

    if (k == 1) {
        return {search_nn(p)};
    }

    const auto q = to_xyz(p);

    Neighbours result;
    {
        util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
        result = shared_->kdtree->k_nearest_neighbours(q, k);
    }

    sort(result);
    return result;
}


Search::Neighbours Search::search_radius(const Point& p, double radius) const {
    check_radius(radius);

    const auto q = to_xyz(p);

    Neighbours result;
    {
        util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
        result = shared_->kdtree->find_in_sphere(q, radius);
    }

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
    if (knn.empty() || knn.back().distance - knn.front().distance > tolerance) {
        return knn;
    }

    return search_radius(p, knn.back().distance + tolerance);
}


PointXYZ Search::to_xyz(const Point& p) const {
    if (std::holds_alternative<PointXYZ>(p)) {
        return std::get<PointXYZ>(p);
    }

    if (const auto* q = std::get_if<PointLonLat>(&p); q != nullptr) {
        return shared_->to_xyz->fwd(*q);
    }

    throw exception::SearchError("Search: unsupported point type (supported: PointXYZ, PointLonLat)", Here());
}


void Search::print(std::ostream& out) const {
    out << "Search[kdtree=" << *shared_->kdtree << "]";
}


}  // namespace eckit::geo
