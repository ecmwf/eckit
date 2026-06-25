/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to itr by virtue of its status as an intergovernmental organisation nor
 * does itr submit to any jurisdiction.
 */


#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/grid/Unstructured.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


void cleanup() {
    if (const auto path = LibEcKitGeo::cacheGrid(); path.exists()) {
        path.unlink(true);
    }
}


CASE("unstructured_ll") {
    cleanup();

    const std::string name = "custom-grid";
    const std::vector<double> lats{0, 1, static_cast<double>(std::rand() % 3)};
    const std::vector<double> lons{static_cast<double>(std::rand() % 3), 1, 2};

    std::unique_ptr<const Grid> a(new grid::Unstructured(lons, lats));  // registers uid only
    std::unique_ptr<const Grid> b(
        GridFactory::build(spec::Custom{{"longitudes", lons}, {"latitudes", lats}, {"grid", name}}));  // registers name
    std::unique_ptr<const Grid> c(
        GridFactory::build(spec::Custom{{"longitudes", lons}, {"latitudes", lats}, {"name", name}}));
    std::unique_ptr<const Grid> d(GridFactory::make_from_string("{grid:" + name + "}"));
    std::unique_ptr<const Grid> e(GridFactory::make_from_string("{grid:" + a->uid() + "}"));
    std::unique_ptr<const Grid> f(GridFactory::make_from_string("{uid:" + a->uid() + "}"));
    std::unique_ptr<const Grid> g(new grid::Unstructured(lons, lats));

    EXPECT(a->uid() == b->uid());  // a doesn't have a name, b does
    EXPECT(*b == *c);
    EXPECT(*b == *d);
    EXPECT(*b == *e);
    EXPECT(*b == *f);
    EXPECT(b->uid() == g->uid());  // b has a name, g doesn't

    cleanup();
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
