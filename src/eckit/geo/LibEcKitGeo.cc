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

#include "eckit/config/Resource.h"
#include "eckit/eckit_version.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/eckit_geo_config.h"


namespace eckit {


REGISTER_LIBRARY(LibEcKitGeo);


LibEcKitGeo::LibEcKitGeo() :
    Library("eckit_geo") {}


LibEcKitGeo& LibEcKitGeo::instance() {
    static LibEcKitGeo lib;
    return lib;
}


PathName LibEcKitGeo::etcGrid() {
    static const PathName path{
        LibResource<std::string, LibEcKitGeo>("eckit-geo-grid;$ECKIT_GEO_GRID", "~eckit/etc/eckit/geo/grid.yaml")};
    return path;
}


bool LibEcKitGeo::caching() {
    static const bool yes{LibResource<bool, LibEcKitGeo>("eckit-geo-caching;$ECKIT_GEO_CACHING",
                                                         eckit_HAVE_GEO_CACHING != 0 ? true : false)};
    return yes;
}


std::string LibEcKitGeo::cacheDir() {
    static std::string path = PathName{
        LibResource<PathName, LibEcKitGeo>("eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH", eckit_GEO_CACHE_PATH)};
    return path;
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
