/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>

#include "eckit/types/FloatCompare.h"
#include "eckit/value/Value.h"

#define ECKIT_TESTING_SELF_REGISTER_CASES 0
#include "eckit/testing/Test.h"

// Disable warnings for old-style casts in these tests. They are intentional
#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif

using namespace eckit::testing;

namespace eckit_test {  // Not eckit namespace on purpose to test downstream usage of macros

typedef std::vector<Test> Tests;

//----------------------------------------------------------------------------------------------------------------------

// These unit tests test the eckit testing framework itself.

//----------------------------------------------------------------------------------------------------------------------


// We need some global counters to keep count of some of the CASE functionality
std::vector<int> global_counters(5, 0);


void ThrowStd() {
    throw std::exception();
}


Tests tests = {{CASE("CASE with no EXPECT passes"){

               }},

               {CASE("EXPECT macros are defined correctly"){EXPECT(true);
EXPECT_NOT(false);
EXPECT_EQUAL(1, 1);
EXPECT_NOT_EQUAL(1, 2);
EXPECT_NO_THROW({ bool b = true; });
EXPECT_THROWS(throw std::exception());
EXPECT_THROWS_AS(throw std::exception(), std::exception);
EXPECT_MSG(1 == 1, [=]() { std::cerr << eckit::Colour::red << "1 != 1" << eckit::Colour::reset << std::endl; };);
}  // namespace eckit_test
}  // namespace eckit
,

    {CASE("EXPECT does not cause the test to fail on success"){

        Test pass = {CASE("P"){EXPECT(true);
}
}
;

std::vector<std::string> f;
bool ret = pass.run(TestVerbosity::Silent, f);
if (!ret || f.size() != 0) {
    throw eckit::testing::TestException("Unexpected error encountered in EXPECT", Here());
}
}
}
,

    {CASE("EXPECT causes an error to be reported on failure"){

        Test fail = {CASE("F"){EXPECT(false);
}
}
;

std::vector<std::string> f;
bool ret = fail.run(TestVerbosity::Silent, f);
if (ret || f.size() == 0) {
    throw eckit::testing::TestException("No error reported when EXPECT failed", Here());
}
if (ret || f.size() > 1) {
    throw eckit::testing::TestException("Too many errors reported when EXPECT failed", Here());
}
}
}
,

    {CASE("EXPECT succeeds for success (true) and failure (false)"){Tests pass = {{CASE("P"){EXPECT(true);
}
}
}
;
Tests fail = {{CASE("F"){EXPECT(false);
}
}
}
;

EXPECT(0 == run(pass, TestVerbosity::Silent));
EXPECT(1 == run(fail, TestVerbosity::Silent));
}
}
,

    {CASE("EXPECT succeeds for integer comparison"){EXPECT(7 == 7);
EXPECT(7 != 8);
EXPECT(7 >= 6);
EXPECT(7 <= 8);
EXPECT(7 > 6);
EXPECT(7 < 8);
EXPECT_NOT(7 == 8);
EXPECT_NOT(7 != 7);
EXPECT_NOT(7 <= 6);
EXPECT_NOT(7 >= 8);
EXPECT_NOT(7 < 6);
EXPECT_NOT(7 > 8);
}
}
, {CASE("Expect succeeds for integer vs. real comparison"){EXPECT(7.0 == 7);
EXPECT(7.0 != 8);
EXPECT(7 == 7.0);
EXPECT(7 != 8.0);

EXPECT_NOT(7.0 == 8);
EXPECT_NOT(7 != 7.0);
}
}
,

    {CASE("Expect succeeds for string comparison"){

        std::string a("a");
std::string b("b");

EXPECT(a == a);
EXPECT(a != b);
EXPECT(b >= a);
EXPECT(a <= b);
EXPECT(b > a);
EXPECT(a < b);

EXPECT_NOT(a == b);
EXPECT_NOT(a != a);
EXPECT_NOT(b <= a);
EXPECT_NOT(a >= b);
EXPECT_NOT(b < a);
EXPECT_NOT(a > b);
}
}
,

    {CASE("Expect expression RHS can use * / % + -"){EXPECT(7 == 1 * 7);
EXPECT(7 == 7 / 1);
EXPECT(0 == 7 % 1);
EXPECT(7 == 1 + 6);
EXPECT(7 == 8 - 1);
}
}
,

    {CASE("Expect expression LHS can use * / % + -"){EXPECT(1 * 7 == 7);
EXPECT(7 / 1 == 7);
EXPECT(7 % 1 == 0);
EXPECT(1 + 6 == 7);
EXPECT(8 - 1 == 7);
}
}
, {CASE("run() returns the correct failure count"){Tests pass = {{CASE("P"){EXPECT(1 == 1);
}
}
}
;
Tests fail_1 = {{CASE("F1"){EXPECT(0 == 1);
}
}
}
;
Tests fail_3 = {{CASE("F1"){EXPECT(0 == 1);
}
}
, {CASE("F2"){EXPECT(0 == 1);
}
}
, {
    CASE("F3") { EXPECT(0 == 1); }
}
}
;

EXPECT(0 == run(pass, TestVerbosity::Silent));
EXPECT(1 == run(fail_1, TestVerbosity::Silent));
EXPECT(3 == run(fail_3, TestVerbosity::Silent));
}
}
, {CASE("run() returns -1 if no CASE() is found"){Tests empty = {};
EXPECT(-1 == run(empty, TestVerbosity::Silent));
}
}
, {CASE("A fresh fixture is created for each section"){

      int i = 7;

SECTION("S1") {
    i = 42;
}
SECTION("S2") {
    EXPECT(i == 7);
}
}
}
, {CASE("Deprecated SETUP macro may be used."){

      SETUP("Context"){int i = 7;

SECTION("S1") {
    i = 42;
}
SECTION("S2") {
    EXPECT(i == 7);
}
}
}
}
, {CASE("Fixture setup runs one more time than the number of sections."){

      ++global_counters[0];

SECTION("S1") {
    ++global_counters[1];
}
SECTION("S2") {
    ++global_counters[2];
}
}
}
, {CASE("Sections run correctly inside a for loop"){

      ++global_counters[3];

for (int j = 0; j < 10; j++) {
    std::stringstream ss;
    ss << "test-" << j;
    SECTION(ss.str()) { ++global_counters[4]; }
}
}
}
, {CASE("Expect runs multiple times in for-loop"){

      int i = 0;
for (int j = 0; j < 10; j++)
    EXPECT(i++ == j);
EXPECT(i == 10);
}
}
, {CASE("Collections compare correctly"){

      SETUP("Create Collections"){std::vector<int> a = {1, 2, 3};
std::vector<int> b = {1, 2, 3};

SECTION("Compare Collections") {
    EXPECT(a == b);
    EXPECT(a <= b);
    EXPECT(a >= b);
    EXPECT_NOT(a != b);
    EXPECT_NOT(a > b);
    EXPECT_NOT(a < b);
    a[0] = 0;
    EXPECT_NOT(a == b);
    EXPECT_NOT(a >= b);
    EXPECT(a <= b);
    EXPECT(a < b);
    EXPECT(a != b);
    a[0] = 2;
    EXPECT_NOT(a == b);
    EXPECT_NOT(a <= b);
    EXPECT(a >= b);
    EXPECT(a > b);
    EXPECT(a != b);
}
}
}
}
, {CASE("Test comparisons of c-style arrays"){

      std::vector<int> a = {1, 2, 3};
std::vector<int> b = {1, 2, 3};
int arr[3]         = {1, 2, 3};

std::vector<std::string> s = {"1", "22", "333"};
std::string arr_s[3]       = {"1", "22", "333"};

std::vector<double> d1 = {1., 2., 3.};
std::vector<double> d2 = {1., 2., 3.};

// Check basic comparisons and initializations
EXPECT(make_view(a.data(), a.data() + 3) == make_view(b.data(), b.data() + 3));
EXPECT(make_view(a.data(), 3) == make_view(b.data(), 3));
EXPECT(make_view(a.data(), a.data() + 3) == make_view(arr, arr + 3));
EXPECT(make_view(a.data(), 3) == make_view(arr, 3));
EXPECT(make_view(a) == make_view(arr, arr + 3));
EXPECT(make_view(s) == make_view(arr_s, arr_s + 3));
EXPECT(make_view(s.data(), s.data() + 3) == make_view(arr_s, arr_s + 3));
EXPECT(make_view(a) == make_view(b.data(), b.data() + 3));
EXPECT(make_view(a) == make_view(b));
EXPECT(make_view(d1.data(), 3) == make_view(d2.data(), 3));
EXPECT(make_view(d1.data(), 3) == make_view(d2.data(), 3));

EXPECT(make_view(a.data(), a.data() + 3).size() == 3);
EXPECT(make_view(a.data(), 3).size() == 3);
EXPECT(make_view(a).size() == a.size());
EXPECT(make_view(s).size() == s.size());

// Check sub-array comparisons
EXPECT(make_view(a.data(), a.data() + 2) == make_view(b.data(), b.data() + 2));
EXPECT(make_view(&a[1], &a[1] + 2) == make_view(&b[1], &b[1] + 2));
EXPECT(make_view(&a[1], &a[1] + 2) != make_view(b.data(), b.data() + 2));        // values not equal
EXPECT(make_view(a.data(), a.data() + 3) != make_view(b.data(), b.data() + 2));  // not same size
EXPECT(make_view(a.data(), 2) == make_view(b.data(), 2));
EXPECT(make_view(a.data(), 2) == make_view(b.data(), 2));

// Check != is the same as !(==)
EXPECT(!(make_view(a.data(), a.data() + 3) != make_view(b.data(), b.data() + 3)));
EXPECT(!(make_view(a.data(), a.data() + 3) == make_view(b.data(), b.data() + 2)));  // not same size
EXPECT(!(make_view(&a[1], &a[1] + 2) == make_view(b.data(), b.data() + 2)));        // values not equal

// Check comparisons against std::vector
EXPECT(a == make_view(b));
EXPECT(make_view(a) == b);

// Check type recognition of make_view vs explicit constructor
EXPECT(ArrayView<int>(a) == make_view(a));
EXPECT(ArrayView<std::string>(s) == make_view(s));

// Check approximately equals
d2[2] = 2.99;
EXPECT(is_approximately_equal(make_view(d1), make_view(d2), 0.02));
EXPECT(!is_approximately_equal(make_view(d1), make_view(d2), 0.0001));  // array vs array
EXPECT(!is_approximately_equal(d1, d2, 0.0001));                        // vector vs vector
EXPECT(!is_approximately_equal(make_view(d1), d2, 0.0001));             // array vs vector
EXPECT(!is_approximately_equal(d1, make_view(d2), 0.0001));             // vector vs array
}
}
,
}
;  // end of tests


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


using eckit_test::global_counters;


int main(int argc, char* argv[]) {

    int retval1 = 0;

    if (global_counters.size() != 5 || std::any_of(global_counters.begin(), global_counters.end(), [](int a) { return a != 0; })) {
        eckit::Log::info() << "Global counters incorrectly configured" << std::endl;
        retval1 = 1;
    }

    eckit::Main::initialise(argc, argv);
    int retval2 = eckit::testing::run_tests(eckit_test::tests, argc, argv);

    int retval3 = 0;
    if (global_counters[0] != 3 || global_counters[1] != 1 || global_counters[2] != 1 || global_counters[3] != 11 || global_counters[4] != 10) {

        eckit::Log::info() << "Global counters incorrect. Relevant tests FAILED" << std::endl;
        eckit::Log::info() << "Global counters: " << global_counters << std::endl;
        retval3 = 1;
    }

    return std::abs(retval1) + std::abs(retval2) + std::abs(retval3);
}
