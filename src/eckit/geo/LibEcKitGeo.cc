/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/LibEcKitGeo.h"

#include <regex>

#include "eckit/config/Resource.h"
#include "eckit/eckit_version.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/utils/StringTools.h"


namespace eckit {


static std::vector<PathName> paths(const std::string& rsrc, const std::string& rsrc_default,
                                   const std::string& path_in_cache) {
    std::vector<PathName> ps;
    for (const auto& s : StringTools::split(":", LibResource<std::string, LibEcKitGeo>(rsrc, rsrc_default))) {
        ps.emplace_back(s, true);
    }

    if (LibEcKitGeo::caching()) {
        ps.emplace_back(PathName{LibEcKitGeo::cacheDir(), true} / path_in_cache);
    }

    return ps;
}


static void purge_path_recursive(const PathName& p) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    p.children(files, dirs);

    for (auto& f : files) {
        f.unlink();
    }

    for (auto& d : dirs) {
        purge_path_recursive(d);
    }

    if (p.exists()) {
        p.rmdir();
    }
}

static void purge_path(const PathName& p) {
    if (p.exists()) {
        try {
            purge_path_recursive(p);
        }
        catch (const Exception& e) {
            throw SeriousBug("LibEcKitGeo: failed to remove path '" + p + "': " + e.what(), Here());
        }
    }
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


std::string LibEcKitGeo::cacheDir() {
    static const std::string path = PathName{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH", eckit_GEO_CACHE_PATH),
        true};
    return path;
}


void LibEcKitGeo::purgeCacheDir() {
    purge_path_recursive(cacheDir());
}


void LibEcKitGeo::purgeCacheArea() {
    purge_path(cacheArea());
}


void LibEcKitGeo::purgeCacheGrid() {
    purge_path(cacheGrid());
}


void LibEcKitGeo::purgeCacheProjection() {
    purge_path(cacheProjection());
}


std::string LibEcKitGeo::url(const std::string& url_path) {
    static const std::regex has_scheme(R"(^[a-zA-Z][a-zA-Z0-9+\-.]*://)");

    static const std::string url_prefix(LibResource<std::string, LibEcKitGeo>(
        "eckit-geo-share-url-prefix;$ECKIT_GEO_SHARE_URL_PREFIX", eckit_GEO_SHARE_URL_PREFIX));
    auto sep =
        !url_prefix.empty() && url_prefix.back() != '/' && !url_path.empty() && url_path.front() != '/' ? "/" : "";

    return std::regex_search(url_path, has_scheme) ? url_path : url_prefix + sep + url_path;
}


bool LibEcKitGeo::proj() {
    static const bool yes{
        LibResource<bool, LibEcKitGeo>("eckit-geo-projection-proj;$ECKIT_GEO_PROJECTION_PROJ",
                                       (eckit_HAVE_PROJ != 0) && (eckit_HAVE_GEO_PROJECTION_PROJ_DEFAULT != 0))};
    return yes;
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
