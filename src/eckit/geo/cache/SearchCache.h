// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/cache/InMemoryCache.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo {
class Grid;
class Search;
}  // namespace eckit::geo


namespace eckit::geo::cache {


/**
 * @brief Searches shared in the process, by grid and k-d tree configuration (see Search), within a capacity
 * @details Searches of other behaviours share the cached k-d tree. Capacity is from resource
 * eckit-geo-search-cache-capacity;$ECKIT_GEO_SEARCH_CACHE_CAPACITY, as "memory[,shared]" (bytes).
 */
class SearchCache final : public InMemoryCache<Search> {
public:

    // -- Methods

    std::shared_ptr<const Search> get(const Grid&, const spec::Spec& = spec::Custom{});

    // -- Class methods

    static SearchCache& instance();

private:

    // -- Constructors

    SearchCache();
};


}  // namespace eckit::geo::cache
