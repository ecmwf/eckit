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

#include "eckit/geo/grid/ORCA.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


const std::string GRID   = "ORCA2_T";
const Grid::uid_type UID = "d5bde4f52ff3a9bea5629cd9ac514410";
const std::vector<long> SHAPE{182, 149};


CASE("spec") {
    std::unique_ptr<spec::Spec> spec(GridFactory::make_spec(spec::Custom({{"uid", UID}})));

    EXPECT(spec->get_string("type") == "ORCA");
    EXPECT(spec->get_string("name") == "ORCA2");
    EXPECT(spec->get_string("arrangement") == "T");
    EXPECT(spec->get_string("uid") == UID);
    EXPECT(spec->get_long_vector("shape") == SHAPE);

    std::unique_ptr<const Grid> grid1(GridFactory::make_from_string("{uid:" + UID + "}"));

    EXPECT(grid1->size() == SHAPE[0] * SHAPE[1]);
    EXPECT(grid1->uid() == UID);

    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"uid", UID}})));

    EXPECT(grid2->size() == SHAPE[0] * SHAPE[1]);
    EXPECT(grid2->uid() == UID);

    grid::ORCA grid3(UID);

    const std::string expected_spec_str = R"({"grid":")" + GRID + R"("})";
    Log::info() << "'" << static_cast<const Grid&>(grid3).spec_str() << "'" << std::endl;

    EXPECT(grid3.uid() == UID);
    EXPECT(grid3.calculate_uid() == UID);
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
    grid::ORCA grid4(UID);

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == grid4);
    EXPECT(grid4 == *grid1);

    EXPECT(grid2->uid() == grid3->calculate_uid());
    EXPECT(grid3->uid() == grid2->calculate_uid());
}


CASE("catalog") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid:" + GRID + "}"));
    std::string intgrid;

    EXPECT(grid->catalog().get("intgrid", intgrid) && !intgrid.empty());
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
