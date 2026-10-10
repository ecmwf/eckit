// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Search.h"

#include <algorithm>
#include <map>
#include <ostream>
#include <string>

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


using Tree = search::Tree;


std::string tree_type(const spec::Spec& spec) {
    return spec.get_string("search-tree",
                           spec.get_bool("caching", LibEcKitGeo::caching()) ? LibEcKitGeo::searchTree() : "memory");
}


bool fast_build(const spec::Spec& spec) {
    return spec.get_bool("search-fast-build", LibEcKitGeo::searchFastBuild());
}


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
    std::unique_ptr<Tree> tree;
    std::unique_ptr<const projection::LonLatToXYZ> to_xyz;

    // queries update the tree statistics
    util::recursive_mutex mutex;
};


template <typename Fill>
void Search::build(const std::string& uid, size_t size, const Spec& spec, const Fill& fill) {
    auto& tree = shared_->tree;
    tree.reset(geo::search::TreeFactory::build(tree_type(spec), uid, size));

    util::lock_guard<Tree> lock(*tree);

    if (tree->ready()) {
        Log::debug() << "Search: loaded " << *tree << std::endl;
        return;
    }

    Trace trace("Search: build " + uid);

    std::vector<Tree::Value> values;
    fill(values);
    ASSERT(values.size() == size);

    if (fast_build(spec)) {
        tree->build(values);
    }
    else {
        for (const auto& value : values) {
            tree->insert(value);
        }
    }

    tree->commit();
}


Search::Search(const Grid& grid, const Spec& spec) : shared_(std::make_shared<Shared>()) {
    configure(spec);

    // (lon, lat) queries are on the figure of the grid points, as Grid::to_xyz
    const auto& figure = grid.projection().source_figure();
    shared_->to_xyz    = std::make_unique<projection::LonLatToXYZ>(figure.a(), figure.b());

    // the tree depends on the grid points and the figure they're converted on
    const auto uid = MD5{grid.uid() + shared_->to_xyz->spec_str()}.digest();

    build(uid, grid.size(), spec, [&grid](std::vector<Tree::Value>& values) {
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

    build(points_uid(points), points.size(), spec, [&points](std::vector<Tree::Value>& values) {
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
    return shared_->tree->size();
}


Search::MemoryUsage Search::footprint() const {
    return shared_->tree->footprint();
}


const search::Tree& Search::tree() const {
    return *shared_->tree;
}


std::unique_ptr<spec::Custom> Search::spec_tree(const Spec& spec) {
    auto tree = std::make_unique<spec::Custom>();

    if (spec.has("search-tree")) {
        tree->set("search-tree", spec.get_string("search-tree"));
    }

    for (const std::string key : {"caching", "search-fast-build"}) {
        if (spec.has(key)) {
            tree->set(key, spec.get_bool(key));
        }
    }

    return tree;
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
    };

    const auto name = spec.get_string("search", "nn");
    auto it         = MODES.find(name);
    if (it == MODES.end()) {
        throw exception::SearchError("Search: unknown search '" + name + "'", Here());
    }

    mode_ = it->second;

    if (mode_ == Mode::KNN || mode_ == Mode::KNN_OR_RADIUS || mode_ == Mode::KNN_AND_RADIUS) {
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
    }

    NOTIMP;
}


Search::Neighbour Search::search_nn(const Point& p) const {
    const auto q = to_xyz(p);

    util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
    return shared_->tree->nearest_neighbour(q);
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
        result = shared_->tree->k_nearest_neighbours(q, k);
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
        result = shared_->tree->find_in_sphere(q, radius);
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
    out << "Search[tree=" << *shared_->tree << "]";
}


}  // namespace eckit::geo
