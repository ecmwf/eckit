// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/SearchCache.h"

#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"


namespace eckit::geo::cache {


SearchCache& SearchCache::instance() {
    static SearchCache cache;
    return cache;
}


SearchCache::SearchCache() : InMemoryCache(MemoryUsage{LibEcKitGeo::searchCacheCapacity()}) {}


std::shared_ptr<const Search> SearchCache::get(const Grid& grid, const spec::Spec& spec) {
    const auto tree = Search::spec_tree(spec);
    const auto search =
        InMemoryCache::get(grid.uid() + tree->str(), [&]() { return std::make_shared<const Search>(grid, *tree); });

    if (!Search::spec_has_behaviour(spec)) {
        return search;
    }

    // the cached search stays in use (not evicted) while its tree is shared
    return {new Search(*search, spec), [search](const Search* ptr) { delete ptr; }};
}


}  // namespace eckit::geo::cache
