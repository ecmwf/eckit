// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/SearchCache.h"

#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"


namespace eckit::geo::cache {


SearchCache& SearchCache::instance() {
    static SearchCache cache;
    return cache;
}


std::unique_ptr<eckit::cache::FileCache> SearchCache::make_file_cache(const eckit::cache::Roots& roots,
                                                                      const std::string& name, bool throw_on_miss) {
    eckit::cache::Loader::Options options;
    options.magic            = MAGIC;
    options.huge_pages       = LibEcKitGeo::sharedMemoryHugePages();
    options.round_huge_pages = LibEcKitGeo::sharedMemoryRoundHugePages();

    return std::make_unique<eckit::cache::FileCache>(roots, name, VERSION, ".kdtree", options, throw_on_miss);
}


const eckit::cache::FileCache& SearchCache::file_cache() {
    static const auto cache = make_file_cache(LibEcKitGeo::cacheRoots(), "search", LibEcKitGeo::cacheThrowOnMiss());
    return *cache;
}


SearchCache::SearchCache(eckit::cache::MemoryCaches& group, const eckit::cache::MemoryUsage& capacity,
                         std::string_view policy, const Search::Storage& storage) :
    MemoryCache(group, capacity, policy), storage_(storage) {}


SearchCache::SearchCache() :
    SearchCache(LibEcKitGeo::memoryCaches(), LibEcKitGeo::searchCacheCapacity(), LibEcKitGeo::searchCachePolicy(),
                Search::Storage::library()) {}


std::shared_ptr<const Search> SearchCache::get(const Grid& grid, const spec::Spec& spec) {
    const auto kdtree = Search::spec_kdtree(spec);
    const auto search =
        MemoryCache::get(Search::uid(grid, *kdtree) + kdtree->str(), Search::estimate(grid.size(), storage_),
                         [&]() { return std::make_shared<const Search>(grid, *kdtree, storage_); });

    if (!Search::spec_has_behaviour(spec)) {
        return search;
    }

    // the cached search stays in use (not evicted) while its k-d tree is shared
    return {new Search(*search, spec), [search](const Search* ptr) { delete ptr; }};
}


}  // namespace eckit::geo::cache
