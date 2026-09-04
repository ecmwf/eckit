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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/grid/SphericalHarmonics.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("sh") {
    grid::SphericalHarmonics a(1);

    EXPECT(a.truncation() == 1);
    EXPECT(a.size() == 6);
    EXPECT(a.size() == grid::SphericalHarmonics::number_of_real_coefficients(1));
    EXPECT(a.size() == grid::SphericalHarmonics::number_of_complex_coefficients(1) * 2);
    EXPECT(a.spec_str() == R"({"grid":"T1"})");

    grid::SphericalHarmonics b(1279);

    EXPECT(b.truncation() == 1279);
    EXPECT(b.size() == 1639680);
    EXPECT(b.size() == grid::SphericalHarmonics::number_of_real_coefficients(1279));
    EXPECT(b.size() == grid::SphericalHarmonics::number_of_complex_coefficients(1279) * 2);
    EXPECT(b.spec_str() == R"({"grid":"T1279"})");

    EXPECT(b == *std::unique_ptr<const Grid>(GridFactory::build(spec::Custom{{"grid", "t1279"}})));
    EXPECT(b == *std::unique_ptr<const Grid>(GridFactory::make_from_string("{type: sh, truncation: 1279}")));

    EXPECT_THROWS_AS((void)GridFactory::build(spec::Custom{{"type", "sh"}, {"truncation", 0}}), exception::SpecError);
    EXPECT_THROWS_AS((void)GridFactory::build(spec::Custom{{"type", "sh"}, {"truncation", -1}}), exception::SpecError);
    EXPECT_THROWS_AS((void)GridFactory::make_from_string("{type: sh, truncation: 0}"), exception::SpecError);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
