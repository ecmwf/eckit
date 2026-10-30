// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/KDTree.h"

#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/cache/Loader.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/geo/search/KDTreeMemory.h"


namespace eckit::geo::search {


namespace {


[[noreturn]] void unknown(const std::string& loader) {
    std::ostringstream msg;
    msg << "KDTreeFactory: unknown '" << loader << "', choices are: ";

    const char* sep = "";
    for (const auto& name : eckit::cache::LoaderFactory::names()) {
        if (KDTreeFactory::has(name)) {
            msg << sep << name;
            sep = ", ";
        }
    }
    throw exception::SearchError(msg.str(), Here());
}


}  // namespace


std::ostream& operator<<(std::ostream& out, const Neighbour& n) {
    return out << "Neighbour[point=" << n.point << ",index=" << n.index << ",distance=" << n.distance << "]";
}


KDTree::KDTree(size_t size) : size_(size) {
    if (size_ == 0) {
        throw exception::SearchError("KDTree: cannot build without points", Here());
    }
}


KDTree::~KDTree() = default;


std::vector<KDTree::Value> KDTree::values(const Fill& fill) const {
    std::vector<Value> values;
    values.reserve(size_);
    fill(values);

    if (values.size() != size_) {
        throw exception::SearchError(
            "KDTree: expected " + std::to_string(size_) + " values, got " + std::to_string(values.size()), Here());
    }

    return values;
}


KDTree* KDTreeFactory::build(const std::string& loader, const std::string& uid, size_t size) {
    // in memory, a k-d tree has its own layout (nodes allocated separately)
    if (loader == "memory") {
        return new KDTreeMemory(size);
    }

    if (!has(loader)) {
        unknown(loader);
    }

    std::unique_ptr<KDTreeCache> cached(new KDTreeCache(loader, uid, size));

    // with nowhere to write, use a cache file only if found, else build in memory (unless a miss is an error)
    if (LibEcKitGeo::cacheWritablePaths().empty() && !LibEcKitGeo::cacheThrowOnMiss() && !cached->found()) {
        return new KDTreeMemory(size);
    }

    return cached.release();
}


KDTree::MemoryUsage KDTreeFactory::estimate(const std::string& loader, size_t size) {
    if (loader == "memory") {
        return KDTreeMemory::estimate(size);
    }

    if (has(loader)) {
        return KDTreeCache::estimate(loader, size);
    }

    unknown(loader);
}


bool KDTreeFactory::has(const std::string& loader) {
    return loader == "memory" || KDTreeCache::loads(loader);
}


}  // namespace eckit::geo::search
