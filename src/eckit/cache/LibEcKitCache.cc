// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/LibEcKitCache.h"

#include <algorithm>

#include "eckit/eckit_version.h"


namespace eckit {


REGISTER_LIBRARY(LibEcKitCache);


LibEcKitCache::LibEcKitCache() : Library("eckit_cache") {}


LibEcKitCache& LibEcKitCache::instance() {
    static LibEcKitCache lib;
    return lib;
}


const void* LibEcKitCache::addr() const {
    return this;
}


std::string LibEcKitCache::version() const {
    return eckit_version_str();
}


std::string LibEcKitCache::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    return sha1.empty() ? "not available" : sha1.substr(0, std::min(count, 40U));
}


}  // namespace eckit
