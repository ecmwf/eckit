// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <vector>

#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/MemoryCache.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


// see the test environment: ECKIT_CACHE_PATH=/eckit/a:/eckit/b, ECKIT_CACHE_TOTAL_CACHE_MEMORY_FOOTPRINT=1000,10


CASE("LibEcKitCache::cacheRoots: the given paths first, then eckit's") {
    EXPECT(LibEcKitCache::cachePath() == "/eckit/a:/eckit/b");

    const std::vector<PathName> paths{"/first", "/eckit/a", "/eckit/b"};
    EXPECT(LibEcKitCache::cacheRoots("/first").paths() == paths);

    const std::vector<PathName> eckit{"/eckit/a", "/eckit/b"};
    EXPECT(LibEcKitCache::cacheRoots().paths() == eckit);
}


CASE("LibEcKitCache::totalMemoryCapacity: the default capacity of a group of caches") {
    EXPECT(LibEcKitCache::totalMemoryCapacity() == MemoryUsage(1000, 10));
    EXPECT(MemoryCaches{}.capacity() == MemoryUsage(1000, 10));
    EXPECT((MemoryCaches{MemoryUsage{1, 2}}.capacity() == MemoryUsage(1, 2)));
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
