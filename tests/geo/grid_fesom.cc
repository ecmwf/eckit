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
#include <utility>
#include <vector>

#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/grid/unstructured/FESOM.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


static const Grid::uid_t UID_C = "e548b74fa53eef5ab412c6061330f043";  // {grid:pi_C}
static const Grid::uid_t UID_N = "bdc49d97a27e389fb86decd08a185c2f";  // {grid:pi_N}
static const size_t SHAPE_N    = 3140;


CASE("caching") {
    if (LibEcKitGeo::caching()) {
        using Cache = cache::MemoryCache;

        SECTION("Grid::build_from_uid") {
            spec::Custom spec({{"uid", UID_N}});

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
    std::unique_ptr<Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", UID_N}})));

    EXPECT(spec->get_string("type") == "FESOM");
    EXPECT(spec->get_string("fesom_arrangement") == "N");
    EXPECT(spec->get_string("fesom_uid") == UID_N);
    EXPECT(spec->get_unsigned("shape") == SHAPE_N);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID_N + "}"));

    EXPECT(grid1->size() == SHAPE_N);
    EXPECT(grid1->uid() == UID_N);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID_N}})));

    EXPECT(grid2->size() == SHAPE_N);
    EXPECT(grid2->uid() == UID_N);

    grid::unstructured::FESOM grid3(UID_N);

    const std::string expected_spec_str = R"({"grid":"pi_N","uid":")" + UID_N + R"("})";

    EXPECT(grid3.uid() == UID_N);
    EXPECT(grid3.calculate_uid() == UID_N);
    EXPECT(static_cast<const Grid&>(grid3).spec_str() == expected_spec_str);

    EXPECT(grid1->spec_str() == grid2->spec_str());

    std::unique_ptr<const Grid> grid4(GridFactory::build(spec::Custom({{"grid", "pi_N"}})));
    EXPECT(grid4->spec_str() == expected_spec_str);

    std::unique_ptr<const Grid> grid5(GridFactory::build(spec::Custom({{"uid", UID_N}})));

    EXPECT(*grid4 == *grid5);
}


CASE("equals") {
    for (const auto& p : std::vector<std::pair<std::string, std::string>>{{UID_N, "pi_N"}, {UID_C, "pi_C"}}) {
        const auto& uid  = p.first;
        const auto& grid = p.second;

        std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + uid + "}"));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", uid}})));
        std::unique_ptr<const Grid> grid3(GridFactory::build(spec::Custom({{"grid", grid}})));
        grid::unstructured::FESOM grid4(uid);

        EXPECT(*grid1 == *grid2);
        EXPECT(*grid2 == *grid3);
        EXPECT(*grid3 == grid4);
        EXPECT(grid4 == *grid1);

        EXPECT(grid2->uid() == grid3->calculate_uid());
        EXPECT(grid3->uid() == grid2->calculate_uid());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
