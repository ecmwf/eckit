// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/KDTree.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <numeric>
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


std::vector<size_t> KDTree::query_order(const std::vector<Point>& points) {
    std::vector<size_t> order(points.size());
    std::iota(order.begin(), order.end(), 0);

    if (points.size() < 2) {
        return order;
    }

    // coordinates as 21-bit integers, in the points bounding box
    Point min = points.front();
    Point max = points.front();
    for (const auto& p : points) {
        min = {std::min(min.X(), p.X()), std::min(min.Y(), p.Y()), std::min(min.Z(), p.Z())};
        max = {std::max(max.X(), p.X()), std::max(max.Y(), p.Y()), std::max(max.Z(), p.Z())};
    }

    auto quantise = [](double x, double lo, double hi) -> std::uint64_t {
        constexpr double top = (1 << 21) - 1;
        return hi > lo ? static_cast<std::uint64_t>((x - lo) / (hi - lo) * top) : 0;
    };

    // the bits of a 21-bit integer, spread to every third bit
    auto spread = [](std::uint64_t v) {
        v = (v | v << 32) & 0x1f00000000ffffULL;
        v = (v | v << 16) & 0x1f0000ff0000ffULL;
        v = (v | v << 8) & 0x100f00f00f00f00fULL;
        v = (v | v << 4) & 0x10c30c30c30c30c3ULL;
        v = (v | v << 2) & 0x1249249249249249ULL;
        return v;
    };

    // Morton code: coordinate bits interleaved, so points near in space are mostly near in order
    std::vector<std::uint64_t> code;
    code.reserve(points.size());
    for (const auto& p : points) {
        code.push_back(spread(quantise(p.X(), min.X(), max.X())) | spread(quantise(p.Y(), min.Y(), max.Y())) << 1 |
                       spread(quantise(p.Z(), min.Z(), max.Z())) << 2);
    }

    std::stable_sort(order.begin(), order.end(), [&code](size_t a, size_t b) { return code[a] < code[b]; });
    return order;
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
