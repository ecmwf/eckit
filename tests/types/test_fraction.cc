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
#include <limits>
#include <utility>

#include "eckit/log/Log.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Constructing fractions") {
    // 0

    EXPECT(Fraction(0, 1) == Fraction());

    EXPECT_THROWS_AS(Fraction(0, 0), std::exception);  // this prints a backtrace wi Assertion failed

    // negative number

    EXPECT(Fraction(-1, 2) == Fraction(3, -6));

    // decimals

    EXPECT(Fraction(0.16) == Fraction(16, 100));
    EXPECT(Fraction(0.1616) == Fraction(1616, 10000));

    // 5 / 7

    EXPECT(Fraction(0.714285) != Fraction(5, 7));

    EXPECT(Fraction(0.7142857142) != Fraction(5, 7));

    EXPECT(Fraction(0.71428571428) == Fraction(5, 7));
    EXPECT(Fraction(0.714285714285) == Fraction(5, 7));
    EXPECT(Fraction(0.714285714286) == Fraction(5, 7));

    EXPECT(Fraction(0.714285714285714285) == Fraction(5, 7));

    // 1 / 6

    EXPECT(Fraction(0.166) != Fraction(1, 6));
    EXPECT(Fraction(0.1666) != Fraction(1, 6));
    EXPECT(Fraction(0.16666) != Fraction(1, 6));
    EXPECT(Fraction(0.166666) != Fraction(1, 6));
    EXPECT(Fraction(0.1666666) != Fraction(1, 6));
    EXPECT(Fraction(0.16666666) != Fraction(1, 6));
    EXPECT(Fraction(0.166666666) != Fraction(1, 6));

    EXPECT(Fraction(0.16666666666) == Fraction(1, 6));
    EXPECT(Fraction(0.166666666666) == Fraction(1, 6));
    EXPECT(Fraction(0.1666666666666) == Fraction(1, 6));
    EXPECT(Fraction(0.16666666666666) == Fraction(1, 6));
    EXPECT(Fraction(0.166666666666666) == Fraction(1, 6));
    EXPECT(Fraction(0.1666666666666666) == Fraction(1, 6));

    // 1 / 3

    EXPECT(Fraction(0.3333333333) == Fraction(1, 3));

    EXPECT(Fraction(0.3333333333333333) == Fraction(1, 3));

    // 1 / 1

    EXPECT(Fraction(0.9999999999999999) == Fraction(1, 1));
    EXPECT(Fraction(0.9999999999999999) == Fraction(10, 10));

    // 7 / 10..

    EXPECT(Fraction(0.7) == Fraction(7, 10));
    EXPECT(Fraction(0.07) == Fraction(7, 100));
    EXPECT(Fraction(0.0000007) == Fraction(7, 10000000));

    // operations

    EXPECT(Fraction(1, 3) + Fraction(2, 3) == Fraction(10, 10));
    EXPECT(Fraction(1, 3) - Fraction(2, 6) == Fraction(0, 10));

    EXPECT(Fraction(1, 3) * 3 == Fraction(1));

    EXPECT(-Fraction(1, 3) == Fraction(1, -3));

    EXPECT(2 * Fraction(1, 3) == Fraction(2, 3));

    Fraction a(1, 3);
    Fraction b(3, 28);
    EXPECT(a + b == Fraction(37, 84));
    EXPECT(a - b == Fraction(19, 84));
    EXPECT(a * b == Fraction(1, 28));
    EXPECT(a / b == Fraction(28, 9));

    EXPECT(a > b);
    EXPECT(a != b);
    EXPECT_NOT(a < b);
    EXPECT_NOT(a == b);

    EXPECT(Fraction("1/3") == Fraction(1, 3));
    EXPECT(double(Fraction("1/3")) == 1.0 / 3.0);

    EXPECT(Fraction("1") == Fraction(1));
    EXPECT(Fraction("1.2") == Fraction(12, 10));
    EXPECT(Fraction("1e-6") == Fraction(1, 1000000));
    EXPECT(Fraction("1e+6") == Fraction(1000000, 1));
    EXPECT(Fraction("1.2e+6") == Fraction(1200000, 1));

    EXPECT(Fraction(M_PI) > Fraction(M_E));
    EXPECT(Fraction(M_E) > Fraction(M_SQRT2));

    std::cout << "pi = " << (M_PI - double(Fraction(M_PI))) << std::endl;
    std::cout << "e = " << (M_E - double(Fraction(M_E))) << std::endl;
    std::cout << "sqrt2 = " << (M_SQRT2 - double(Fraction(M_SQRT2))) << std::endl;

    // EXPECT(Fraction(M_PI), Fraction(1200000, 1));
    {
        Fraction west(-12), east(1.2), increment(1.2);

        Fraction f(west);
        while (f < east) {
            f += increment;
        }
        EXPECT(f == east);
    }

    {
        Fraction west("-77"), east("7"), increment("0.7");

        Fraction f(west);
        while (f < east) {
            f += increment;
        }
        EXPECT(f == east);
    }


    EXPECT(Fraction(241.85) / Fraction(0.175) == 1382);
    EXPECT(Fraction(284.025) / Fraction(0.175) == 1623);


    // EXPECT(Fraction(5, 3).intergralPart() == 1);
    // EXPECT(Fraction(5, 3).decimalPart() == Fraction(1, 3));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Overflow during comparisons") {

    {
        Fraction A(5934563467713522511, 13567822205000000);  // 437.39985519021053
        Fraction B(8624662771, 19718023);                    // 437.3999751902105

        EXPECT(A < B);
        EXPECT(A <= B);
        EXPECT(A != B);
        EXPECT(B > A);
        EXPECT(B >= A);
    }

    {
        using limits = std::numeric_limits<Fraction::value_type>;
        std::streamsize p(Log::info().precision(16));

        Fraction A(limits::max() - 6, limits::max() - 1);
        Fraction B(limits::max() - 2, limits::max() - 1);

        Log::info() << limits::max() - 6 << " ?= " << double(limits::max() - 6) << std::endl;
        Log::info() << limits::max() - 2 << " ?= " << double(limits::max() - 2) << std::endl;

        /// @note this demonstrates that numerators/denominators have a lossy representation in double
        EXPECT(double(limits::max() - 6) == double(limits::max() - 2));

        EXPECT(A != B);

        /// @note these fail due to the lossy conversion to double of numerators/denominators
        //    EXPECT(A <  B);
        //    EXPECT(A <= B);
        //    EXPECT(B >  A);
        //    EXPECT(B >= A);

        Log::info() << "Max denominator" << Fraction::max_denominator() << std::endl;

        Fraction U(1, 1);

        /// @note these fail due to the lossy conversion to double of numerators/denominators

        EXPECT(A != U);
        EXPECT(A < U);
        EXPECT(A <= U);
        EXPECT(U > A);
        EXPECT(U >= A);

        EXPECT(B != U);
        EXPECT(B < U);
        EXPECT(B <= U);
        EXPECT(U > B);
        EXPECT(U >= B);

        Log::info().precision(p);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Regression (Fraction <=> Fraction)") {

    Fraction A(-34093871309, 378680550);  // -90.033331
    Fraction B(-12621809, 378680550);     //  -0.3333102

    EXPECT(A < B);
    EXPECT(A <= B);
    EXPECT(A != B);
    EXPECT(B > A);
    EXPECT(B >= A);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Regression (double <=> Fraction)") {

    double A(253.07432399999999006467987783253);
    Fraction B(/*253.07435716708162*/ 1591682572, 6289387);

    EXPECT(A < B);
    EXPECT(A <= B);
    EXPECT(A != B);
    EXPECT(B > A);
    EXPECT(B >= A);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Regression (Fraction <=> double)") {

    Fraction A(/*-253.07435716708162*/ -1591682572, 6289387);
    double B(-253.07432399999999006467987783253);

    EXPECT(A < B);
    EXPECT(A <= B);
    EXPECT(A != B);
    EXPECT(B > A);
    EXPECT(B >= A);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Values known to have problematic conversion to fraction") {

    auto values = std::vector<double>{0.47718059708975263};
    std::streamsize p(Log::debug().precision(16));
    for (auto value : values) {

        Log::debug() << "Test " << value << "..." << std::endl;
        Log::debug() << "Test " << value << " = " << Fraction(value) << std::endl;
    }
    Log::debug().precision(p);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Fraction inverse") {

    for (auto& test : {
             std::make_pair(Fraction{1, 2}, Fraction{2, 1}),
             std::make_pair(Fraction{1, 3}, Fraction{3, 1}),
         }) {

        Log::debug() << "Test (" << test.first << ")**-1 = " << test.second << std::endl;
        EXPECT(test.first.inverse() == test.second);

        Log::debug() << "Test " << test.first << " = (" << test.second << ")**-1" << std::endl;
        EXPECT(test.first == test.second.inverse());
    }

    Fraction zero(0);
    Log::debug() << "Test (" << zero << ")**-1 (throw BadValue)" << std::endl;
    EXPECT_THROWS_AS(zero.inverse(), BadValue);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Fraction precision") {
    auto old = Log::debug().precision(16);

    const std::vector<Fraction::value_type> primes{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    const std::vector<Fraction::value_type> den{10, 100, 1000, 10000, 100000, 1000000, 10000000, 2, 4, 8, 16, 32, 64};

    for (auto& prime : primes) {
        Fraction exact_p(1, prime);
        Fraction exact_m(-exact_p);

        for (auto& p : den) {
            const Fraction precision{1, p};
            double value_p = round(double(exact_p) * p) / p;
            double value_m = round(double(exact_m) * p) / p;

            Log::debug() << "Test  " << exact_p << " ~=  " << value_p << " +- " << precision << endl;
            EXPECT(Fraction::abs(exact_p - Fraction(value_p, precision)) < precision);

            Log::debug() << "Test " << exact_m << " ~= " << value_m << " +- " << precision << endl;
            EXPECT(Fraction::abs(exact_m - Fraction(value_m, precision)) < precision);
        }
    }

    Log::debug().precision(old);
}


CASE("String to double to fraction to double to string") {
    auto old = Log::debug().precision(16);

    Translator<std::string, double> s2d;

    std::string s("-17.9229");

    Fraction f(s2d(s));

    std::ostringstream oss;
    oss << double(f);

    EXPECT(oss.str() == s);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
