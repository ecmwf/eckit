// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/KDTree.h"

#include <map>
#include <ostream>
#include <sstream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo::search {


std::ostream& operator<<(std::ostream& out, const Neighbour& n) {
    return out << "Neighbour[point=" << n.point << ",index=" << n.index << ",distance=" << n.distance << "]";
}


KDTree::KDTree(const std::string& uid, size_t size) : uid_(uid), size_(size) {
    if (size_ == 0) {
        throw exception::SearchError("KDTree: cannot build without points", Here());
    }
}


KDTree::~KDTree() = default;


namespace {


struct Registry {
    util::recursive_mutex mutex;
    std::map<std::string, const KDTreeFactory*> loaders;
};


// constructed on first registration, so it outlives the (static) builders
Registry& registry() {
    static Registry r;
    return r;
}


}  // namespace


KDTreeFactory::KDTreeFactory(const std::string& loader) : loader_(loader) {
    auto& r = registry();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    if (!r.loaders.try_emplace(loader, this).second) {
        throw exception::SearchError("KDTreeFactory: duplicate '" + loader + "'", Here());
    }
}


KDTreeFactory::~KDTreeFactory() {
    auto& r = registry();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    r.loaders.erase(loader_);
}


const KDTreeFactory& KDTreeFactory::get(const std::string& loader) {
    auto& r = registry();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    if (auto it = r.loaders.find(loader); it != r.loaders.end()) {
        return *it->second;
    }

    std::ostringstream msg;
    list(msg << "KDTreeFactory: unknown '" << loader << "', choices are: ");
    throw exception::SearchError(msg.str(), Here());
}


KDTree* KDTreeFactory::build(const std::string& loader, const std::string& uid, size_t size) {
    return get(loader).make(uid, size);
}


KDTree::MemoryUsage KDTreeFactory::estimate(const std::string& loader, size_t size) {
    return get(loader).make_estimate(size);
}


bool KDTreeFactory::has_type(const std::string& loader) {
    auto& r = registry();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    return r.loaders.find(loader) != r.loaders.end();
}


std::ostream& KDTreeFactory::list(std::ostream& out) {
    auto& r = registry();
    util::lock_guard<util::recursive_mutex> lock(r.mutex);

    const char* sep = "";
    for (const auto& [loader, factory] : r.loaders) {
        out << sep << loader;
        sep = ", ";
    }
    return out;
}


}  // namespace eckit::geo::search
