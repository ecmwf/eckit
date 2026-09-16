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


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Grid.h"
#include "eckit/geo/Projection.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("RegularXY") {
    if (!ProjectionFactory::has_type("proj")) {
        return;
    }


    SECTION("EPSG:8857 (Equal Earth), global and coarse") {
        // the grid is defined on the projection plane, in metres
        std::unique_ptr<const Grid> grid(
            GridFactory::make_from_string(R"({"type": "regular_xy", "grid": [500000.0, 500000.0],)"
                                          R"( "bounding_box_xy": [-17000000.0, -8000000.0, 17000000.0, 8000000.0],)"
                                          R"( "projection": {"type": "proj", "target": "EPSG:8857"}})"));

        const std::vector<size_t> shape{33, 69};

        EXPECT_EQUAL(grid->type(), "regular_xy");
        EXPECT(grid->shape() == shape);
        EXPECT_EQUAL(grid->size(), 69 * 33);

        // a geographic bounding box, unlike the spec's own frame
        EXPECT(grid->boundingBox().north() < NORTH_POLE.lat());
    }


    SECTION("spec round-trip") {
        // NOTE: without 'type' a two-valued 'grid' would be taken for a regular_ll's increments, in degrees
        for (const auto& name : std::vector<std::string>{"swisslv95", "swisslv03"}) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(name));
            std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));

            EXPECT_EQUAL(same->spec_str(), grid->spec_str());
            EXPECT(same->shape() == grid->shape());
        }
    }


    SECTION("swisslv95 isn't snapped to multiples of its increment") {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("swisslv95"));

        const std::vector<size_t> shape{294, 429};

        EXPECT(grid->shape() == shape);
        EXPECT_EQUAL(grid->spec().get_double_vector("bounding_box_xy")[1], 1040500.);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
