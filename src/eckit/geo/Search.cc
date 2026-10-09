// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Search.h"

#include <algorithm>
#include <map>
#include <ostream>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Trace.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


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


const Projection* make_projection_to_xyz(const Figure& figure) {
#if eckit_HAVE_PROJ
    try {
        const auto ellipsoid = figure.proj_str();
        return new projection::PROJ("+proj=longlat " + ellipsoid, "+proj=cart " + ellipsoid);
    }
    catch (const std::exception& e) {
        // e.g. PROJ without its database, the fallback is equivalent
        Log::debug() << "Search: PROJ not used: " << e.what() << std::endl;
    }
#endif

    return new projection::LonLatToXYZ(figure.a(), figure.b());
}


const Projection* make_projection_to_xyz(const Grid& grid) {
#if eckit_HAVE_PROJ
    if (const auto* proj = dynamic_cast<const projection::PROJ*>(&grid.projection());
        proj != nullptr && proj->source_point_coordinates() == point_coordinates<PointLonLat>()) {
        try {
            // (lon, lat) are on the PROJ source CRS, whose ellipsoid can differ from the grid figure (target CRS)
            const projection::PROJ geographic(proj->source(), proj->source());
            return new projection::PROJ(proj->source(), "+proj=cart " + geographic.figure().proj_str());
        }
        catch (const std::exception& e) {
            Log::debug() << "Search: PROJ not used: " << e.what() << std::endl;
        }
    }
#endif

    return make_projection_to_xyz(grid.figure());
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
    std::unique_ptr<const Projection> to_xyz;

    // queries update the tree statistics, and PROJ objects are not thread-safe
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

    shared_->to_xyz.reset(make_projection_to_xyz(grid));
    const auto& projection = *shared_->to_xyz;

    // the tree depends on the grid points and their conversion
    const auto uid = MD5{grid.uid() + projection.spec_str()}.digest();

    build(uid, grid.size(), spec, [&grid, &projection](std::vector<Tree::Value>& values) {
        // all at once, as projections convert vectors efficiently (PROJ)
        const auto [lat, lon] = grid.to_latlons();
        const auto xyz        = projection.fwd(lon, lat);
        ASSERT(xyz.size() == 3 && xyz[0].size() == lat.size());

        values.reserve(lat.size());
        for (size_t i = 0; i < lat.size(); ++i) {
            values.emplace_back(PointXYZ{xyz[0][i], xyz[1][i], xyz[2][i]}, i);
        }
    });
}


Search::Search(const std::vector<PointXYZ>& points, const Spec& spec) : shared_(std::make_shared<Shared>()) {
    configure(spec);

    shared_->to_xyz.reset(make_projection_to_xyz(*std::unique_ptr<const Figure>(FigureFactory::build(spec))));

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

    if (std::holds_alternative<PointLonLat>(p)) {
        util::lock_guard<util::recursive_mutex> lock(shared_->mutex);
        return std::get<PointXYZ>(shared_->to_xyz->fwd(p));
    }

    throw exception::SearchError("Search: unsupported point type (supported: PointXYZ, PointLonLat)", Here());
}


void Search::print(std::ostream& out) const {
    out << "Search[tree=" << *shared_->tree << "]";
}


}  // namespace eckit::geo
