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

#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/grid/unstructured/ICON.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


static const std::string GRID = "ICON_55_R02B05";
static const Grid::uid_t UID  = "e234e01a8556e9a84bcb42361d2f24e0";
static const std::vector<long> SHAPE{2656};


CASE("caching") {
    if (LibEcKitGeo::caching()) {
        using Cache = cache::MemoryCache;

        SECTION("Grid::build_from_uid") {
            spec::Custom spec{{"uid", UID}};

            const auto footprint_1 = Cache::total_footprint();

            std::cout << spec << std::endl;
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
    std::unique_ptr<Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", UID}})));

    EXPECT(spec->get_string("type") == "ICON");
    EXPECT(spec->get_string("name") == GRID);
    EXPECT(spec->get_string("icon_number_of_grid_used") == "55");
    EXPECT(spec->get_string("icon_type") == "hrz_regional");
    EXPECT(spec->get_string("icon_arrangement") == "T");
    EXPECT(spec->get_string("icon_uid") == UID);
    EXPECT(spec->get_long_vector("shape") == SHAPE);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID + "}"));

    EXPECT(grid1->size() == SHAPE[0]);
    EXPECT(grid1->uid() == UID);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID}})));

    EXPECT(grid2->size() == SHAPE[0]);
    EXPECT(grid2->uid() == UID);

    grid::unstructured::ICON grid3(UID);

    const std::string expected_spec_str = R"({"grid":")" + GRID + R"("})";
    Log::info() << "'" << static_cast<const Grid&>(grid3).spec_str() << "'" << std::endl;

    EXPECT(grid3.uid() == UID);
    EXPECT(static_cast<const Grid&>(grid3).spec_str() == expected_spec_str);

    EXPECT(grid1->spec_str() == grid2->spec_str());

    std::unique_ptr<const Grid> grid4(GridFactory::build(spec::Custom({{"grid", GRID}})));

    EXPECT(grid4->spec_str() == expected_spec_str);

    std::unique_ptr<const Grid> grid5(GridFactory::build(spec::Custom({{"uid", UID}})));

    EXPECT(*grid4 == *grid5);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID + "}"));
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID}})));
    std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", GRID}})));
    grid::unstructured::ICON grid4(UID);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == grid4);
    EXPECT(grid4 == *grid1);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
