// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit_exceptions.h"

#include "RustPoint.h"
#include "SearchWrapper.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/cache/SearchCache.h"

#include <sstream>

namespace eckit_bridge {

namespace {

Neighbour to_neighbour(const eckit::geo::Search::Neighbour& n) {
    return {n.index, n.distance, RustPoint::to_raw(n.point)};
}

rust::Vec<Neighbour> to_neighbours(const eckit::geo::Search::Neighbours& neighbours) {
    rust::Vec<Neighbour> out;
    out.reserve(neighbours.size());
    for (const auto& n : neighbours) {
        out.push_back(to_neighbour(n));
    }
    return out;
}

MemoryUsage to_usage(const eckit::cache::MemoryUsage& usage) {
    return {usage.memory(), usage.shared()};
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

rust::Vec<Neighbour> SearchWrapper::search(RawPoint point) const {
    return to_neighbours(search_->search(RustPoint::from_raw(point)));
}

Neighbour SearchWrapper::search_nn(RawPoint point) const {
    return to_neighbour(search_->search_nn(RustPoint::from_raw(point)));
}

rust::Vec<Neighbour> SearchWrapper::search_knn(RawPoint point, size_t k) const {
    return to_neighbours(search_->search_knn(RustPoint::from_raw(point), k));
}

rust::Vec<Neighbour> SearchWrapper::search_radius(RawPoint point, double radius) const {
    return to_neighbours(search_->search_radius(RustPoint::from_raw(point), radius));
}

rust::Vec<Neighbour> SearchWrapper::search_knn_or_radius(RawPoint point, size_t k, double radius) const {
    return to_neighbours(search_->search_knn_or_radius(RustPoint::from_raw(point), k, radius));
}

rust::Vec<Neighbour> SearchWrapper::search_knn_and_radius(RawPoint point, size_t k, double radius) const {
    return to_neighbours(search_->search_knn_and_radius(RustPoint::from_raw(point), k, radius));
}

size_t SearchWrapper::size() const {
    return search_->size();
}

MemoryUsage SearchWrapper::footprint() const {
    return to_usage(search_->footprint());
}

rust::String SearchWrapper::to_string() const {
    std::ostringstream out;
    out << *search_;
    return rust::String(out.str());
}

// ============== Cache ==============

SearchCacheStatistics SearchWrapper::cache_statistics() {
    const auto& cache = eckit::geo::cache::SearchCache::instance();
    const auto stats  = cache.statistics();
    return {stats.hits, stats.misses, stats.evictions, cache.size(), to_usage(cache.usage())};
}

void SearchWrapper::cache_clear() {
    eckit::geo::cache::SearchCache::instance().clear();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
