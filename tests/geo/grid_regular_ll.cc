/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <memory>

#include "eckit/geo/grid/regular/RegularLL.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using namespace geo;


CASE("") {
    std::unique_ptr<Grid> global(new grid::regular::RegularLL(spec::Custom{{{"grid", std::vector<double>{1, 1}}}}));

    size_t global_size = global->size();
    EXPECT_EQUAL(global_size, 360 * 181);

    std::unique_ptr<Grid> local(new grid::regular::RegularLL(
        spec::Custom{{{"grid", std::vector<double>{2, 1}}, {"area", std::vector<double>{10, 1, 1, 10}}}}));

    size_t local_size = local->size();
    EXPECT_EQUAL(local_size, 5 * 10);

    std::unique_ptr<Grid> almost_global(new grid::regular::RegularLL(
        spec::Custom{{{"grid", std::vector<double>{1, 1}}, {"area", std::vector<double>{89.5, 0., -89.5, 359.1}}}}));

    size_t almost_global_size = almost_global->size();
    EXPECT_EQUAL(almost_global_size, 360 * 180);
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
