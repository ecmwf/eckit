// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/LibEcKitGeo.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/MemoryCache.h"
#include "eckit/config/Resource.h"
#include "eckit/eckit_version.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/cache/CropCache.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/log/JSON.h"
#include "eckit/utils/StringTools.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit {


static std::vector<PathName> paths(const std::string& rsrc, const std::string& rsrc_default,
                                   const std::string& path_in_cache) {
    auto vs = StringTools::split(":", LibResource<std::string, LibEcKitGeo>(rsrc, rsrc_default));
    std::vector<PathName> ps(vs.begin(), vs.end());

    // as cached, if found
    for (const auto& root : LibEcKitGeo::cacheRoots().paths()) {
        ps.emplace_back(root / path_in_cache);
    }

    return ps;
}


REGISTER_LIBRARY(LibEcKitGeo);


LibEcKitGeo::LibEcKitGeo() : Library("eckit_geo") {}


LibEcKitGeo& LibEcKitGeo::instance() {
    static LibEcKitGeo lib;
    return lib;
}


std::vector<PathName> LibEcKitGeo::shareArea() {
    static const auto ps = paths("eckit-geo-share-area;$ECKIT_GEO_SHARE_AREA", eckit_GEO_SHARE_AREA, "area.yaml");
    return ps;
}


std::vector<PathName> LibEcKitGeo::shareGrid() {
    static const auto ps = paths("eckit-geo-share-grid;$ECKIT_GEO_SHARE_GRID", eckit_GEO_SHARE_GRID, "grid.yaml");
    return ps;
}


std::vector<PathName> LibEcKitGeo::shareProjection() {
    static const auto ps =
        paths("eckit-geo-share-projection;$ECKIT_GEO_SHARE_PROJECTION", eckit_GEO_SHARE_PROJECTION, "projection.yaml");
    return ps;
}


PathName LibEcKitGeo::cacheArea() {
    static const auto p = PathName(cacheDir()) / "area.yaml";
    return p;
}


PathName LibEcKitGeo::cacheGrid() {
    static const auto p = PathName(cacheDir()) / "grid.yaml";
    return p;
}


PathName LibEcKitGeo::cacheProjection() {
    static const auto p = PathName(cacheDir()) / "projection.yaml";
    return p;
}


bool LibEcKitGeo::caching() {
    static const bool yes{
        LibResource<bool, LibEcKitGeo>("eckit-geo-caching;$ECKIT_GEO_CACHING", eckit_HAVE_GEO_CACHING)};
    return yes;
}


const cache::Roots& LibEcKitGeo::cacheRoots() {
    static const cache::Roots roots{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH", eckit_GEO_CACHE_PATH),
        caching()};
    return roots;
}


std::vector<PathName> LibEcKitGeo::cacheReadablePaths() {
    return cacheRoots().readable_paths();
}


std::vector<PathName> LibEcKitGeo::cacheWritablePaths() {
    return cacheRoots().writable_paths();
}


std::string LibEcKitGeo::cacheDir() {
    const auto writable = cacheWritablePaths();
    if (writable.empty()) {
        throw UserError(
            "LibEcKitGeo: no writable cache path (eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH), or not "
            "caching (eckit-geo-caching;$ECKIT_GEO_CACHING)",
            Here());
    }
    return writable.front().asString();
}


std::string LibEcKitGeo::searchCacheLoader() {
    // not caching: memory (nothing written)
    static const std::string loader{caching() ? std::string{LibResource<std::string, LibEcKitGeo>(
                                                    "eckit-geo-search-loader;$ECKIT_GEO_SEARCH_LOADER", "mapped-file")}
                                              : std::string{"memory"}};
    return loader;
}


bool LibEcKitGeo::sharedMemoryHugePages() {
    static const bool yes{
        LibResource<bool, LibEcKitGeo>("eckit-geo-shared-memory-hugepages;$ECKIT_GEO_SHARED_MEMORY_HUGEPAGES", false)};
    return yes;
}


bool LibEcKitGeo::sharedMemoryRoundHugePages() {
    static const bool yes{LibResource<bool, LibEcKitGeo>(
        "eckit-geo-shared-memory-round-hugepages;$ECKIT_GEO_SHARED_MEMORY_ROUND_HUGEPAGES", false)};
    return yes;
}


cache::MemoryUsage LibEcKitGeo::searchCacheCapacity() {
    static const cache::MemoryUsage capacity{LibResource<std::string, LibEcKitGeo>(
        "eckit-geo-cache-search-capacity;$ECKIT_GEO_CACHE_SEARCH_CAPACITY", eckit_GEO_CACHE_SEARCH_CAPACITY)};
    return capacity;
}


cache::MemoryUsage LibEcKitGeo::cropCacheCapacity() {
    static const cache::MemoryUsage capacity{LibResource<std::string, LibEcKitGeo>(
        "eckit-geo-cache-crop-capacity;$ECKIT_GEO_CACHE_CROP_CAPACITY", eckit_GEO_CACHE_CROP_CAPACITY)};
    return capacity;
}


std::string LibEcKitGeo::searchCachePolicy() {
    static const std::string policy{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-cache-search-policy;$ECKIT_GEO_CACHE_SEARCH_POLICY", "lru")};
    return policy;
}


std::string LibEcKitGeo::cropCachePolicy() {
    static const std::string policy{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-cache-crop-policy;$ECKIT_GEO_CACHE_CROP_POLICY", "lru")};
    return policy;
}


bool LibEcKitGeo::cacheThrowOnMiss() {
    static const bool yes{
        LibResource<bool, LibEcKitGeo>("eckit-geo-cache-throw-on-miss;$ECKIT_GEO_CACHE_THROW_ON_MISS", false)};
    return yes;
}


cache::MemoryCaches& LibEcKitGeo::memoryCaches() {
    // never destroyed, as the caches (static objects) are destroyed in any order
    static auto* caches = new cache::MemoryCaches;
    return *caches;
}


void LibEcKitGeo::purgeCache() {
    memoryCaches().purge();
}


void LibEcKitGeo::purgeCacheSearch() {
    geo::cache::SearchCache::instance().purge();
}


void LibEcKitGeo::purgeCacheCrop() {
    geo::cache::CropCache::instance().purge();
}


cache::MemoryUsage LibEcKitGeo::cacheFootprint() {
    return memoryCaches().footprint();
}


namespace {


void json_usage(JSON& j, const cache::MemoryUsage& usage) {
    j.startObject();
    j << "memory" << usage.memory() << "shared" << usage.shared();
    j.endObject();
}


template <typename Cache>
void json_memory_cache(JSON& j, const Cache& cache) {
    const auto stats = cache.statistics();

    j.startObject();
    j << "size" << cache.size();
    j << "policy" << cache.policy().name();
    j << "footprint";
    json_usage(j, cache.usage());
    j << "capacity";
    json_usage(j, cache.capacity());
    j << "statistics";
    j.startObject();
    j << "hits" << stats.hits << "misses" << stats.misses << "insertions" << stats.insertions << "evictions"
      << stats.evictions;
    j.endObject();
    j.endObject();
}


/// Total size of the files under a path (bytes)
size_t disk_usage(const PathName& path) {
    if (!path.exists()) {
        return 0;
    }

    if (!path.isDir()) {
        return static_cast<size_t>(path.size());
    }

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    path.children(files, dirs);

    size_t size = 0;
    for (const auto& f : files) {
        size += disk_usage(f);
    }
    for (const auto& d : dirs) {
        size += disk_usage(d);
    }
    return size;
}


}  // namespace


std::string LibEcKitGeo::cacheInfo() {
    const auto writable = cacheWritablePaths();

    std::ostringstream out;
    JSON j(out);

    j.startObject();
    j << "caching" << caching();
    j << "throw_on_miss" << cacheThrowOnMiss();

    j << "paths";
    j.startObject();
    j << "readable";
    j.startList();
    for (const auto& p : cacheReadablePaths()) {
        j << p.asString();
    }
    j.endList();
    j << "writable";
    j.startList();
    for (const auto& p : writable) {
        j << p.asString();
    }
    j.endList();
    j.endObject();

    j << "memory";
    j.startObject();
    j << "footprint";
    json_usage(j, cacheFootprint());
    j << "search";
    json_memory_cache(j, geo::cache::SearchCache::instance());
    j << "crop";
    json_memory_cache(j, geo::cache::CropCache::instance());
    j.endObject();

    j << "search";
    j.startObject();
    j << "loader" << searchCacheLoader();
    j << "shared_memory_huge_pages" << sharedMemoryHugePages();
    j << "shared_memory_round_huge_pages" << sharedMemoryRoundHugePages();
    j.endObject();

    // cache files, where they are written (the first writable path)
    j << "disk";
    j.startObject();
    for (const std::string name : {"search", "crop", "latlon", "download"}) {
        j << name;
        j.startObject();
        j << "size" << (writable.empty() ? 0 : disk_usage(writable.front() / name));
        j.endObject();
    }
    j.endObject();

    j.endObject();
    return out.str();
}


void LibEcKitGeo::removeCacheDir() {
    // nothing written, if nowhere to write
    if (!cacheWritablePaths().empty()) {
        cache::DiskCache::remove(cacheDir());
    }
}


void LibEcKitGeo::removeCacheArea() {
    cache::DiskCache::remove(cacheArea());
}


void LibEcKitGeo::removeCacheGrid() {
    cache::DiskCache::remove(cacheGrid());
}


void LibEcKitGeo::removeCacheProjection() {
    cache::DiskCache::remove(cacheProjection());
}


std::string LibEcKitGeo::url(const std::string& url_path) {
    static const std::string url_prefix(LibResource<std::string, LibEcKitGeo>(
        "eckit-geo-share-url-prefix;$ECKIT_GEO_SHARE_URL_PREFIX", eckit_GEO_SHARE_URL_PREFIX));
    return url(url_path, url_prefix);
}


std::string LibEcKitGeo::url(const std::string& url_path, const std::string& url_prefix) {
    static const std::regex has_scheme(R"(^[a-zA-Z][a-zA-Z0-9+\-.]*://)");

    auto sep =
        !url_prefix.empty() && url_prefix.back() != '/' && !url_path.empty() && url_path.front() != '/' ? "/" : "";

    return std::regex_search(url_path, has_scheme) ? url_path : url_prefix + sep + url_path;
}


bool LibEcKitGeo::projdb_is_available() {
#if eckit_HAVE_PROJ
    return geo::projection::PROJ::projdb_is_available();
#else
    return false;
#endif
}


void LibEcKitGeo::projdb_set_search_paths([[maybe_unused]] const std::string& db_path,
                                          [[maybe_unused]] const std::vector<std::string>& search_paths) {
#if eckit_HAVE_PROJ
    geo::projection::PROJ::projdb_set_search_paths(db_path, search_paths);
#endif
}


void LibEcKitGeo::projdb_reset() {
#if eckit_HAVE_PROJ
    geo::projection::PROJ::projdb_reset();
#endif
}


const void* LibEcKitGeo::addr() const {
    return this;
}


std::string LibEcKitGeo::version() const {
    return eckit_version_str();
}


std::string LibEcKitGeo::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    return sha1.empty() ? "not available" : sha1.substr(0, std::min(count, 40U));
}


}  // namespace eckit
