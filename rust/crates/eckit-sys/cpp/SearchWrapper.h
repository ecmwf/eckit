// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Search bridge: a cached `eckit::geo::Search`.
#pragma once

#include "eckit/geo/Search.h"

#include "rust/cxx.h"

#include <cstddef>
#include <memory>

namespace eckit_bridge {

struct MemoryUsage;
struct Neighbour;
struct RawPoint;
struct SearchCacheStatistics;

//----------------------------------------------------------------------------------------------------------------------

/// A search on the points of a grid, following k-d tree algorithms, for Rust FFI.
///
/// Searches are shared in the process by grid and k-d tree configuration (see
/// `eckit::geo::cache::SearchCache`), which hands them out as shared pointers:
/// holding one keeps the search from being evicted while in use.
class SearchWrapper {
    std::shared_ptr<const eckit::geo::Search> search_;

public:

    explicit SearchWrapper(std::shared_ptr<const eckit::geo::Search> search) : search_(std::move(search)) {}

    /// As configured by the spec (nearest neighbour by default).
    rust::Vec<Neighbour> search(RawPoint point) const;

    Neighbour search_nn(RawPoint point) const;
    rust::Vec<Neighbour> search_knn(RawPoint point, size_t k) const;
    rust::Vec<Neighbour> search_radius(RawPoint point, double radius) const;
    rust::Vec<Neighbour> search_knn_or_radius(RawPoint point, size_t k, double radius) const;
    rust::Vec<Neighbour> search_knn_and_radius(RawPoint point, size_t k, double radius) const;

    size_t size() const;
    MemoryUsage footprint() const;
    rust::String to_string() const;

    // ============== Cache ==============

    static SearchCacheStatistics cache_statistics();
    static void cache_clear();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
