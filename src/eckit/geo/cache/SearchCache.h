// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "eckit/cache/FileCache.h"
#include "eckit/cache/MemoryCache.h"
#include "eckit/geo/Search.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo {
class Grid;
}


namespace eckit::geo::cache {


/**
 * @brief Searches shared in the process, by grid and k-d tree configuration (see Search), within a capacity
 * @details When caching, k-d trees are also shared across processes by cache file (see file_cache()). Searches of
 * other behaviours share the cached k-d tree. Room is made for a search before building or loading its k-d tree (see
 * search::KDTreeFactory::estimate). Capacity is from resource
 * eckit-geo-cache-search-capacity;$ECKIT_GEO_CACHE_SEARCH_CAPACITY, as "process[,shared]" (bytes).
 */
class SearchCache final : public eckit::cache::MemoryCache<Search> {
public:

    // -- Constructors

    /// Searches of k-d trees kept in storage (its file cache outliving this), within a capacity of a group of caches
    SearchCache(eckit::cache::MemoryCaches&, const eckit::cache::MemoryUsage& capacity, std::string_view policy,
                const Search::Storage&);

    // -- Methods

    std::shared_ptr<const Search> get(const Grid&, const spec::Spec& = spec::Custom{});

    // -- Class members

    /// k-d tree cache file layout version (part of its path, see search::KDTreeCache)
    static constexpr int VERSION = 1;

    /// Content kind, where shared between processes (see eckit::cache::Loader::Options)
    static constexpr int MAGIC = 0x6b647472;  // "kdtr"

    // -- Class methods

    static SearchCache& instance();

    /// Cache files of k-d trees (search/VERSION/<uid>.kdtree)
    static const eckit::cache::FileCache& file_cache();

    /// Cache files of k-d trees in these roots (<name>/VERSION/<uid>.kdtree)
    static std::unique_ptr<eckit::cache::FileCache> make_file_cache(const eckit::cache::Roots&, const std::string& name,
                                                                    bool throw_on_miss);

private:

    // -- Constructors

    SearchCache();

    // -- Members

    const Search::Storage storage_;
};


}  // namespace eckit::geo::cache
