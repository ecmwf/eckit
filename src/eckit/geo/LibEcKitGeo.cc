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
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/utils/StringTools.h"


namespace eckit {


REGISTER_LIBRARY(LibEcKitGeo);


LibEcKitGeo::LibEcKitGeo() : Library("eckit_geo") {}


LibEcKitGeo& LibEcKitGeo::instance() {
    static LibEcKitGeo lib;
    return lib;
}


std::vector<PathName> LibEcKitGeo::shareArea() {
    static const auto paths = [](const std::string& s) -> std::vector<PathName> {
        const auto ss = StringTools::split(":", s);
        return {ss.begin(), ss.end()};
    }(LibResource<std::string, LibEcKitGeo>("eckit-geo-share-area;$ECKIT_GEO_SHARE_AREA", eckit_GEO_SHARE_AREA));
    return paths;
}


std::vector<PathName> LibEcKitGeo::shareGrid() {
    static const auto paths = [](const std::string& s) -> std::vector<PathName> {
        const auto ss = StringTools::split(":", s);
        return {ss.begin(), ss.end()};
    }(LibResource<std::string, LibEcKitGeo>("eckit-geo-share-grid;$ECKIT_GEO_SHARE_GRID", eckit_GEO_SHARE_GRID));
    return paths;
}


std::vector<PathName> LibEcKitGeo::shareProjection() {
    static const auto paths = [](const std::string& s) -> std::vector<PathName> {
        const auto ss = StringTools::split(":", s);
        return {ss.begin(), ss.end()};
    }(LibResource<std::string, LibEcKitGeo>("eckit-geo-share-projection;$ECKIT_GEO_SHARE_PROJECTION",
                                                       eckit_GEO_SHARE_PROJECTION));
    return paths;
}


bool LibEcKitGeo::caching() {
    static const bool yes{
        LibResource<bool, LibEcKitGeo>("eckit-geo-caching;$ECKIT_GEO_CACHING", eckit_HAVE_GEO_CACHING)};
    return yes;
}


std::string LibEcKitGeo::cacheDir() {
    static std::string path = PathName{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH", eckit_GEO_CACHE_PATH),
        true};
    return path;
}


std::string LibEcKitGeo::url(const std::string& url_path) {
    struct URLPrefix : std::string {
        URLPrefix() :
            std::string(LibResource<std::string, LibEcKitGeo>("eckit-geo-share-url-prefix;$ECKIT_GEO_SHARE_URL_PREFIX",
                                                              eckit_GEO_SHARE_URL_PREFIX)) {
            if (!empty() && back() != '/') {
                operator+=('/');
            }
        }
    } static const url_prefix;
    static const std::regex has_scheme(R"(^[a-zA-Z][a-zA-Z0-9+\-.]*://)");

    return std::regex_search(url_path, has_scheme) ? url_path : url_prefix + url_path;
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
