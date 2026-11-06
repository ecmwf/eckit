// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/LibEcKitCache.h"

#include <algorithm>

#include "eckit/cache/eckit_cache_config.h"
#include "eckit/config/Resource.h"
#include "eckit/eckit_version.h"


namespace eckit {


REGISTER_LIBRARY(LibEcKitCache);


LibEcKitCache::LibEcKitCache() : Library("eckit_cache") {}


LibEcKitCache& LibEcKitCache::instance() {
    static LibEcKitCache lib;
    return lib;
}


bool LibEcKitCache::throwOnMiss() {
    static const bool yes{LibResource<bool, LibEcKitCache>("eckit-cache-throw-on-miss;$ECKIT_CACHE_THROW_ON_MISS",
                                                           eckit_CACHE_THROW_ON_MISS != 0)};
    return yes;
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
