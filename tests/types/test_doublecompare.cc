/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

bool is_equal(double a, double b, double epsilon, int maxUlps) {
    return eckit::types::is_approximately_equal(a, b, epsilon, maxUlps);
}

bool is_equal(double a, double b, double epsilon) {
    return eckit::types::is_approximately_equal(a, b, epsilon);
}

bool is_equal(double a, double b) {
    return eckit::types::is_approximately_equal(a, b, 0.00001);
}

const double dEps = std::numeric_limits<double>::epsilon();
const double dInf = std::numeric_limits<double>::infinity();
const double sMin = std::numeric_limits<double>::denorm_min();
const double dMin = std::numeric_limits<double>::min();
const double dMax = std::numeric_limits<double>::max();
const double qNaN = std::numeric_limits<double>::quiet_NaN();
const double sNaN = std::numeric_limits<double>::signaling_NaN();

};  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("test_large_numbers") {
    Log::info() << "test_large_numbers" << std::endl;

    EXPECT(is_equal(1000000, 1000000));
    EXPECT(is_equal(1000000, 1000000.00001));
    EXPECT(is_equal(1000000.00001, 1000000));

    EXPECT(!is_equal(1000000.0, 1000001.0));
    EXPECT(!is_equal(1000001.0, 1000000.0));

    // -----------------------------------------------
    EXPECT(is_equal(dMax, dMax));
    EXPECT(is_equal(dMax, dMax, dEps));

    EXPECT(is_equal(dMin, dMin));
    EXPECT(is_equal(dMin, dMin, dEps));
}

CASE("test_negative_large_numbers") {
    Log::info() << "test_negative_large_numbers " << dMin << std::endl;

    EXPECT(is_equal(-1000000, -1000000));
    EXPECT(is_equal(-1000000, -1000000.00001));
    EXPECT(is_equal(-1000000.00001, -1000000));

    EXPECT(!is_equal(-1000000.0, -1000001.0));
    EXPECT(!is_equal(-1000001.0, -1000000.0));

    // -----------------------------------------------
    EXPECT(is_equal(-dMax, -dMax));
    EXPECT(is_equal(-dMax, -dMax, dEps));

    EXPECT(is_equal(-dMin, -dMin));
    EXPECT(is_equal(-dMin, -dMin, dEps));
}

CASE("test_large_numbers_of_opposite_sign") {
    EXPECT(!is_equal(-1000000, 1000000));
    EXPECT(!is_equal(-1000000, 1000000.00001));
    EXPECT(!is_equal(-1000000.00001, 1000000));

    EXPECT(!is_equal(-1000000.0, 1000001.0));
    EXPECT(!is_equal(-1000001.0, 1000000.0));

    // Overflow occurs here in eckit::types::is_approximately_equal
    EXPECT(!is_equal(-dMax, dMax));
    EXPECT(!is_equal(-dMax, dMax, dEps));
}

CASE("test_ulp_around_one") {
    Log::info() << "test_ulp_around_one" << std::endl;

    // ULP distances up to 10 are equal
    // Going right from 1 by eps increases distance by 1
    // Going left from 1 by eps increases distance by 2
    for (int i = 0; i <= 10; ++i) {
        EXPECT(is_equal(1.0 + i * dEps, 1.0, dEps));
        EXPECT(is_equal(1.0, 1.0 + i * dEps, dEps));
        EXPECT(is_equal(1.0 - i * dEps / 2, 1.0, dEps));
        EXPECT(is_equal(1.0, 1.0 - i * dEps / 2, dEps));
    }
    // ULP distances greater 10 are not equal
    EXPECT(!is_equal(1.0 + 11 * dEps, 1.0, dEps));
    EXPECT(!is_equal(1.0, 1.0 + 11 * dEps, dEps));
    EXPECT(!is_equal(1.0 - 11 * dEps / 2, 1.0, dEps));
    EXPECT(!is_equal(1.0, 1.0 - 11 * dEps / 2, dEps));
}

CASE("test_numbers_around_one") {
    Log::info() << "test_numbers_around_one" << std::endl;

    EXPECT(is_equal(1.0000001, 1.0000002));
    EXPECT(is_equal(1.0000002, 1.0000001));

    EXPECT(is_equal(1.12345, 1.12346));
    EXPECT(is_equal(1.12345, 1.12344, 0.001));

    EXPECT(!is_equal(1.0001, 1.0002));
    EXPECT(!is_equal(1.0002, 1.0001));
}

CASE("test_numbers_around_negative_one") {
    Log::info() << "test_numbers_around_negative_one" << std::endl;

    EXPECT(is_equal(-1.0000001, -1.0000002));
    EXPECT(is_equal(-1.0000002, -1.0000001));

    EXPECT(!is_equal(-1.0001, -1.0002));
    EXPECT(!is_equal(-1.0002, -1.0001));
}

CASE("test_numbers_between_one_and_zero") {
    Log::info() << "test_numbers_between_one_and_zero" << std::endl;

    EXPECT(is_equal(0.000000001000001, 0.000000001000002));
    EXPECT(is_equal(0.000000001000002, 0.000000001000001));

    EXPECT(!is_equal(0.00102, 0.00101));
    EXPECT(!is_equal(0.00101, 0.00102));
}

CASE("test_numbers_between_minusone_and_zero") {
    Log::info() << "test_numbers_between_minusone_and_zero" << std::endl;

    EXPECT(is_equal(-0.000000001000001, -0.000000001000002));
    EXPECT(is_equal(-0.000000001000002, -0.000000001000001));

    EXPECT(!is_equal(-0.00102, -0.00101));
    EXPECT(!is_equal(-0.00101, -0.00102));
}

CASE("test_comparisons_involving_zero") {
    Log::info() << "test_comparisons_involving_zero" << std::endl;

    EXPECT(is_equal(0.0, 0.0));
    EXPECT(is_equal(0.0, -0.0));
    EXPECT(is_equal(-0.0, -0.0));

    EXPECT(!is_equal(0.0001, 0.0));
    EXPECT(!is_equal(0.0, 0.0001));
    EXPECT(!is_equal(-0.0001, 0.0));
    EXPECT(!is_equal(0.0, -0.0001));

    EXPECT(is_equal(0.0, 1e-40, 0.01));
    EXPECT(is_equal(1e-40, 0.0, 0.01));
    EXPECT(!is_equal(1e-40, 0.0, 1e-41));
    EXPECT(!is_equal(0.0, 1e-40, 1e-41));

    EXPECT(is_equal(0.0, -1e-40, 0.1));
    EXPECT(is_equal(-1e-40, 0.0, 0.1));
    EXPECT(!is_equal(-1e-40, 0.0, 1e-41));
    EXPECT(!is_equal(0.0, -1e-40, 1e-41));
}

CASE("test_comparisons_involving_infinity") {
    Log::info() << "test_comparisons_involving_infinity" << std::endl;

    if (std::numeric_limits<double>::has_infinity) {

        EXPECT(is_equal(dInf, dInf));
        EXPECT(is_equal(-dInf, -dInf));
        EXPECT(!is_equal(dInf, dMax));
        EXPECT(!is_equal(dMax, dInf));
        EXPECT(!is_equal(-dInf, -dMax));
        EXPECT(!is_equal(-dMax, -dInf));
    }
    else {
        Log::info() << "test_comparisons_involving_infinity NOT VALID on this platform" << std::endl;
    }
}

CASE("test_comparisons_involving_nan") {
    Log::info() << "test_comparisons_involving_nan" << std::endl;

    // The value NaN (Not a Number) is used to represent a value that does not represent a real number.
    // NaN's are represented by a bit pattern with an exponent of all 1s and a non-zero fraction. T
    // there are two categories of NaN: QNaN (Quiet NaN) and SNaN (Signalling NaN).
    //
    // A QNaN is a NaN with the most significant fraction bit set.
    // QNaN's propagate freely through most arithmetic operations.
    // These values pop out of an operation when the result is not mathematically defined.

    // An SNaN is a NaN with the most significant fraction bit clear.
    // It is used to signal an exception when used in operations.
    // SNaN's can be handy to assign to uninitialized variables to trap premature usage.

    // Semantically, QNaN's denote indeterminate operations, while SNaN's denote invalid operations.

    EXPECT(!is_equal(qNaN, qNaN));
    EXPECT(!is_equal(qNaN, 0.0));
    EXPECT(!is_equal(-0.0, qNaN));
    EXPECT(!is_equal(qNaN, -0.0));
    EXPECT(!is_equal(0.0, qNaN));
    EXPECT(!is_equal(qNaN, dInf));
    EXPECT(!is_equal(dInf, qNaN));
    EXPECT(!is_equal(qNaN, dMax));
    EXPECT(!is_equal(dMax, qNaN));
    EXPECT(!is_equal(qNaN, -dMax));
    EXPECT(!is_equal(-dMax, qNaN));
    EXPECT(!is_equal(qNaN, dMin));
    EXPECT(!is_equal(dMin, qNaN));
    EXPECT(!is_equal(qNaN, -dMin));
    EXPECT(!is_equal(-dMin, qNaN));

    EXPECT(!is_equal(sNaN, sNaN));
    EXPECT(!is_equal(sNaN, 0.0));
    EXPECT(!is_equal(-0.0, sNaN));
    EXPECT(!is_equal(sNaN, -0.0));
    EXPECT(!is_equal(0.0, sNaN));
    EXPECT(!is_equal(sNaN, dInf));
    EXPECT(!is_equal(dInf, sNaN));
    EXPECT(!is_equal(sNaN, dMax));
    EXPECT(!is_equal(dMax, sNaN));
    EXPECT(!is_equal(sNaN, -dMax));
    EXPECT(!is_equal(-dMax, sNaN));
    EXPECT(!is_equal(sNaN, dMin));
    EXPECT(!is_equal(dMin, sNaN));
    EXPECT(!is_equal(sNaN, -dMin));
    EXPECT(!is_equal(-dMin, sNaN));
}

CASE("test_comparisons_opposite_side_of_zero") {
    Log::info() << "test_comparisons_opposite_side_of_zero" << std::endl;

    EXPECT(!is_equal(1.000000001, -1.0));
    EXPECT(!is_equal(-1.0, 1.000000001));
    EXPECT(!is_equal(-1.000000001, 1.0));
    EXPECT(!is_equal(1.0, -1.000000001));

    EXPECT(is_equal(10.0 * dMin, 10.0 * -dMin));
    EXPECT(is_equal(10000 * dMin, 10000 * -dMin));
}

CASE("test_comparisons_very_close_to_zero") {
    Log::info() << "test_comparisons_very_close_to_zero" << std::endl;

    EXPECT(is_equal(dMin, -dMin, dEps));
    EXPECT(is_equal(-dMin, dMin, dEps));
    EXPECT(is_equal(dMin, 0, dEps));
    EXPECT(is_equal(0, dMin, dEps));
    EXPECT(is_equal(-dMin, 0, dEps));
    EXPECT(is_equal(0, -dMin, dEps));


    EXPECT(is_equal(0.000000001, -dMin));
    EXPECT(is_equal(0.000000001, dMin));
    EXPECT(is_equal(dMin, 0.000000001));
    EXPECT(is_equal(-dMin, 0.000000001));


    EXPECT(!is_equal(0.000000001, -dMin, 1e-10));
    EXPECT(!is_equal(0.000000001, dMin, 1e-10));
    EXPECT(!is_equal(dMin, 0.000000001, 1e-10));
    EXPECT(!is_equal(-dMin, 0.000000001, 1e-10));
}

CASE("test_comparisons_with_denormal_numbers") {
    Log::info() << "test_comparisons_with_denormal_numbers" << std::endl;

    EXPECT(is_equal(sMin, -sMin, dEps));
    EXPECT(is_equal(-sMin, sMin, dEps));
    EXPECT(is_equal(sMin, 0, dEps));
    EXPECT(is_equal(0, sMin, dEps));
    EXPECT(is_equal(-sMin, 0, dEps));
    EXPECT(is_equal(0, -sMin, dEps));

    const double lMin = dMin - sMin;  // largest denormal number
    EXPECT(is_equal(lMin, -lMin, dEps));
    EXPECT(is_equal(-lMin, lMin, dEps));
    EXPECT(is_equal(lMin, 0, dEps));
    EXPECT(is_equal(0, lMin, dEps));
    EXPECT(is_equal(-lMin, 0, dEps));
    EXPECT(is_equal(0, -lMin, dEps));
}

CASE("test_comparisons_ulps") {
    Log::info() << "test_comparisons_ulps" << std::endl;

    EXPECT(is_equal(dMin, -dMin, 0, 2));
    EXPECT(is_equal(-dMin, dMin, 0, 2));
    EXPECT(is_equal(dMin, 0, 0, 1));
    EXPECT(is_equal(0, dMin, 0, 1));
    EXPECT(is_equal(-dMin, 0, 0, 1));
    EXPECT(is_equal(0, -dMin, 0, 1));

    EXPECT(!is_equal(dMin, -dMin, 0, 1));
    EXPECT(!is_equal(-dMin, dMin, 0, 1));
    EXPECT(!is_equal(dMin, 0, 0, 0));
    EXPECT(!is_equal(0, dMin, 0, 0));
    EXPECT(!is_equal(-dMin, 0, 0, 0));
    EXPECT(!is_equal(0, -dMin, 0, 0));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

#if defined(_CRAYC)
#include <fenv.h>
#else
static int fedisableexcept(int excepts) {
    return 0;
}
static int FE_ALL_EXCEPT = 0;
#endif

int main(int argc, char** argv) {
    fedisableexcept(FE_ALL_EXCEPT);
    return run_tests(argc, argv);
}
