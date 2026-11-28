// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


// see the test environment: ECKIT_GEO_CACHE_PATH=/geo/a:/geo/b, ECKIT_CACHE_PATH=/eckit/a:/eckit/b


CASE("LibEcKitGeo::cacheRoots: eckit::geo's paths first, then eckit's (all under eckit/geo)") {
    const std::vector<PathName> paths{"/geo/a/eckit/geo", "/geo/b/eckit/geo", "/eckit/a/eckit/geo",
                                      "/eckit/b/eckit/geo"};
    EXPECT(LibEcKitGeo::cacheRoots().paths() == paths);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
