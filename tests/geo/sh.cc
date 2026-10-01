// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/SphericalHarmonics.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("sh") {
    EXPECT_THROWS(grid::SphericalHarmonics(-1));

    grid::SphericalHarmonics a(1);

    EXPECT(a.truncation() == 1);
    EXPECT(a.size() == 6);
    EXPECT(a.size() == grid::SphericalHarmonics::number_of_real_coefficients(a.truncation()));
    EXPECT(a.size() == grid::SphericalHarmonics::number_of_complex_coefficients(a.truncation()) * 2);
    EXPECT(a.spec_str() == R"({"grid":"T1"})");

    grid::SphericalHarmonics b(1279);

    EXPECT(b.truncation() == 1279);
    EXPECT(b.size() == 1639680);
    EXPECT(b.size() == grid::SphericalHarmonics::number_of_real_coefficients(b.truncation()));
    EXPECT(b.size() == grid::SphericalHarmonics::number_of_complex_coefficients(b.truncation()) * 2);
    EXPECT(b.spec_str() == R"({"grid":"T1279"})");

    EXPECT(b == *std::unique_ptr<const Grid>(GridFactory::build(spec::Custom{{"grid", "t1279"}})));
    EXPECT(b == *std::unique_ptr<const Grid>(GridFactory::make_from_string("{type: sh, truncation: 1279}")));

    EXPECT_THROWS_AS((void)GridFactory::build(spec::Custom{{"type", "sh"}, {"truncation", 0}}), exception::SpecError);
    EXPECT_THROWS_AS((void)GridFactory::build(spec::Custom{{"type", "sh"}, {"truncation", -1}}), exception::SpecError);
    EXPECT_THROWS_AS((void)GridFactory::make_from_string("{type: sh, truncation: 0}"), exception::SpecError);
}


CASE("name") {
    EXPECT_EQUAL(grid::SphericalHarmonics(1).name(), "T1");
    EXPECT_EQUAL(grid::SphericalHarmonics(1279).name(), "T1279");
    EXPECT(grid::SphericalHarmonics(1).arrangement().empty());
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
