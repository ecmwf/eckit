/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Translator.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Translate strings to integers") {

    Translator<string, size_t> toSize;

    EXPECT(toSize("0") == size_t(0));
    EXPECT(toSize("1") == size_t(1));
    EXPECT(toSize("100") == size_t(100));

    EXPECT(toSize("1KB") == size_t(1024));
    EXPECT(toSize("1MB") == size_t(1024 * 1024));
    EXPECT(toSize("1GB") == size_t(1024 * 1024 * 1024));
    //    EXPECT( toSize("1TB")  == size_t(1024*1024*1024*1024));
    //    EXPECT( toSize("1PB")  == size_t(1024*1024*1024*1024*1024));
    //    EXPECT( toSize("1EB")  == size_t(1024*1024*1024*1024*1024*1024));

    EXPECT(toSize("1KiB") == size_t(1024));
    EXPECT(toSize("1MiB") == size_t(1024 * 1024));
    EXPECT(toSize("1GiB") == size_t(1024 * 1024 * 1024));

    std::cout << toSize("1GiB") << std::endl;
    std::cout << toSize("1TiB") << std::endl;

    EXPECT(toSize("1TiB") == size_t(1LL << 40));
    EXPECT(toSize("1PiB") == size_t(1LL << 50));
    EXPECT(toSize("1EiB") == size_t(1LL << 60));


    //    Translator<string, int> toInt;
    //    Translator<string, unsigned int> toUInt;
    //    Translator<string, long> toLong;
    //    Translator<string, unsigned long> toULong;
}

CASE("Translate strings to double") {
    double half = 0.5;
    double zero = 0.0;
    double hund = 100.0;
    double ten  = 10.;

    Translator<string, double> t;

    EXPECT(half == t("0.5"));

    EXPECT(hund == t("1e2"));
    EXPECT(hund == t("1E2"));
    EXPECT(hund == t("1.e2"));
    EXPECT(hund == t("1.E2"));
    EXPECT(hund == t("1.0e2"));
    EXPECT(hund == t("1.0E2"));
    EXPECT(hund == t("1e002"));
    EXPECT(hund == t("1E002"));
    EXPECT(hund == t("1.0e002"));
    EXPECT(hund == t("1.0E002"));

    EXPECT(zero == t("0"));
    EXPECT(zero == t("0."));
    EXPECT(zero == t("0.0"));
    EXPECT(zero == t("0.00"));
    EXPECT(zero == t("0.0e0"));
    EXPECT(zero == t("0.0e-0"));
    EXPECT(zero == t("0.0e+0"));
    EXPECT(zero == t("+0"));
    EXPECT(zero == t("+0."));
    EXPECT(zero == t("+0.0"));
    EXPECT(zero == t("+0.00"));
    EXPECT(zero == t("+0.0e0"));
    EXPECT(zero == t("+0.0e-0"));
    EXPECT(zero == t("+0.0e+0"));
    EXPECT(zero == t("-0"));
    EXPECT(zero == t("-0."));
    EXPECT(zero == t("-0.0"));
    EXPECT(zero == t("-0.00"));
    EXPECT(zero == t("-0.0e0"));
    EXPECT(zero == t("-0.0e-0"));
    EXPECT(zero == t("-0.0e+0"));

    /// weird cases that actually pass

    EXPECT_NO_THROW(t("inf"));       // inf is acceptable, case insensitive
    EXPECT_NO_THROW(t("INF"));       // INF is acceptable, case insensitive
    EXPECT_NO_THROW(t("infinity"));  // infinity is acceptable, case insensitive
    EXPECT_NO_THROW(t("INFINITY"));  // INFINITY is acceptable, case insensitive

    EXPECT_NO_THROW(t("nan"));  // nan is acceptable, case insensitive
    EXPECT_NO_THROW(t("NAN"));  // NAN is acceptable, case insensitive

    EXPECT(zero == t("0x0"));  // hexadecimal is acceptable
    EXPECT(ten == t("0xA"));   // hexadecimal is acceptable

    /// these should fail ...

    EXPECT_THROWS_AS(t(""), BadParameter);  // empty string

    EXPECT_THROWS_AS(t("0.5 "), BadParameter);     // no spaces accepted -- we are being extra strict
    EXPECT_THROWS_AS(t(" 0.5"), BadParameter);     // no spaces accepted -- we are being extra strict
    EXPECT_THROWS_AS(t(" 0.5   "), BadParameter);  // no spaces accepted -- we are being extra strict

    EXPECT_THROWS_AS(t("1e+10000"), BadParameter);  // overflow,  max ~ 1.79769e+308
    EXPECT_THROWS_AS(t("1e-10000"), BadParameter);  // underflow, min ~ 2.22507e-308

    EXPECT_THROWS_AS(t("0.5a"), BadParameter);
    EXPECT_THROWS_AS(t("foobar"), BadParameter);
    EXPECT_THROWS_AS(t("foo555bar"), BadParameter);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
