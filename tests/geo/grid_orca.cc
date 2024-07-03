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

#include "eckit/geo/Cache.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/grid/ORCA.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


static const Grid::uid_t uid = "d5bde4f52ff3a9bea5629cd9ac514410";
static const std::vector<long> dimensions{182, 149};


CASE("caching") {
    if (LibEcKitGeo::caching()) {
        SECTION("Grid::build_from_uid") {
            spec::Custom spec({{"uid", uid}});

            const auto footprint_1 = Cache::total_footprint();

            std::unique_ptr<const Grid> grid1(GridFactory::build(spec));

            const auto footprint_2 = Cache::total_footprint();
            EXPECT(footprint_1 < footprint_2);

            std::unique_ptr<const Grid> grid2(GridFactory::build(spec));

            EXPECT(footprint_2 == Cache::total_footprint());

            EXPECT(grid1->size() == grid2->size());
        }
    }
}


CASE("spec") {
    std::unique_ptr<Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", uid}})));

    EXPECT(spec->get_string("type") == "ORCA");
    EXPECT(spec->get_string("orca_name") == "ORCA2");
    EXPECT(spec->get_string("orca_arrangement") == "T");
    EXPECT(spec->get_string("orca_uid") == uid);
    EXPECT(spec->get_long_vector("dimensions") == dimensions);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + uid + "}"));

    EXPECT(grid1->size() == dimensions[0] * dimensions[1]);
    EXPECT(grid1->uid() == uid);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", uid}})));

    EXPECT(grid2->size() == dimensions[0] * dimensions[1]);
    EXPECT(grid2->uid() == uid);

    grid::ORCA grid3(uid);

    const std::string expected_spec_str = R"({"type":"ORCA","uid":")" + uid + R"("})";

    EXPECT(grid3.uid() == uid);
    EXPECT(grid3.calculate_uid() == uid);
    EXPECT(static_cast<const Grid&>(grid3).spec_str() == expected_spec_str);

    EXPECT(grid1->spec_str() == grid2->spec_str());

    std::unique_ptr<const Grid> grid4(GridFactory::build(spec::Custom({{"grid", "ORCA2_T"}})));

    EXPECT(grid4->spec_str() == expected_spec_str);

    std::unique_ptr<const Grid> grid5(GridFactory::build(spec::Custom({{"uid", uid}})));

    EXPECT(*grid4 == *grid5);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + uid + "}"));
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", uid}})));
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", uid}})));
    grid::ORCA grid4(uid);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == grid4);
    EXPECT(grid4 == *grid1);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
