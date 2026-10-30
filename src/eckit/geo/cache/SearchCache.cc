// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/SearchCache.h"

#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"


namespace eckit::geo::cache {


SearchCache& SearchCache::instance() {
    static SearchCache cache;
    return cache;
}


const eckit::cache::FileCache& SearchCache::file_cache() {
    static const eckit::cache::FileCache cache(LibEcKitGeo::cacheRoots(), "search", VERSION, ".kdtree", [] {
        eckit::cache::FileCache::Options options;
        options.throw_on_miss           = LibEcKitGeo::cacheThrowOnMiss();
        options.loader.magic            = MAGIC;
        options.loader.huge_pages       = LibEcKitGeo::sharedMemoryHugePages();
        options.loader.round_huge_pages = LibEcKitGeo::sharedMemoryRoundHugePages();
        return options;
    }());
    return cache;
}


SearchCache::SearchCache() :
    MemoryCache(LibEcKitGeo::memoryCaches(), LibEcKitGeo::searchCacheCapacity(), LibEcKitGeo::searchCachePolicy()) {}


std::shared_ptr<const Search> SearchCache::get(const Grid& grid, const spec::Spec& spec) {
    const auto kdtree = Search::spec_kdtree(spec);
    const auto search = MemoryCache::get(Search::uid(grid) + kdtree->str(), Search::estimate(grid.size()),
                                         [&]() { return std::make_shared<const Search>(grid, *kdtree); });

    if (!Search::spec_has_behaviour(spec)) {
        return search;
    }

    // the cached search stays in use (not evicted) while its k-d tree is shared
    return {new Search(*search, spec), [search](const Search* ptr) { delete ptr; }};
}


}  // namespace eckit::geo::cache
