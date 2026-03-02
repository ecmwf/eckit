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


#include "eckit/geo/util.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b) EXPECT(::eckit::types::is_approximately_equal<double>((a), (b), 1e-6))


namespace eckit::geo::test {


/**
 * This case verifies that Floating Point Exceptions (FPEs) don't trigger for the functions:
 * - eckit::geo::util::gaussian_latitudes
 * - eckit::geo::util::gaussian_quadrature_weights
 *
 * FPE trapping is enabled to ensure any FPE-triggering operations are caught.
 * @see ECKIT-674
 */
CASE("gaussian_latitudes") {
    const auto& lats_1 = geo::util::gaussian_latitudes(1, true);

    EXPECT(lats_1.size() == 2);
    EXPECT_APPROX(lats_1.front(), -35.264390);
    EXPECT_APPROX(lats_1.front(), -lats_1.back());

    const auto& lats_2 = geo::util::gaussian_latitudes(80, false);

    EXPECT(lats_2.size() == 2 * 80);
    EXPECT_APPROX(lats_2.front(), 89.141519);
    EXPECT_APPROX(lats_2.front(), -lats_2.back());
}


CASE("gaussian_quadrature_weights") {
    const auto& quad_1 = geo::util::gaussian_quadrature_weights(1);

    EXPECT(quad_1.size() == 2);
    EXPECT_APPROX(quad_1.front(), 0.5);
    EXPECT_APPROX(quad_1.front(), quad_1.back());

    const auto& quad_2 = geo::util::gaussian_quadrature_weights(80);

    EXPECT(quad_2.size() == 2 * 80);
    EXPECT_APPROX(quad_2.front(), 0.000144);
    EXPECT_APPROX(quad_2.front(), quad_2.back());
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    eckit::maths::FloatingPointExceptions::enable_floating_point_exceptions();

    return eckit::testing::run_tests(argc, argv);
}
