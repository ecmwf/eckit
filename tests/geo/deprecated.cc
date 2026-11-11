// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>

#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/testing/Test.h"


// the deprecated API, as used before (e.g. by eccodes and mir)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


namespace eckit::geo::test {


CASE("Grid::iterator") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: O2}"));

    Grid::iterator it = grid->begin();
    EXPECT(it->index() == 0);
    EXPECT(++it);
    EXPECT(it->index() == 1);
}


CASE("ReducedGaussian::octahedral") {
    EXPECT(grid::reduced::ReducedGaussian(2).octahedral());
}


CASE("order::Scan") {
    const order::Scan scan("i+j+");
    EXPECT(scan.order() == "i+j+");
    EXPECT(scan.reorder("i+j-", 3, 2) == order::RegularScan("i+j+").reorder("i+j-", 3, 2));

    EXPECT_THROWS_AS(order::Scan("i?j?"), exception::OrderError);
}


CASE("cache::Download") {
    const cache::Download download(PathName{"download"});
    EXPECT_THROWS(download.to_cached_path("file:///does/not/exist/eckit_test_geo_deprecated"));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
