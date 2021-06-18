/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"
#include "eckit/runtime/Main.h"

#include "eckit/log/Log.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"


#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-function"
#endif

using namespace std;
using namespace eckit;
using namespace eckit::option;
using namespace eckit::types;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Test the options parser
/// @note The options parser normall calls ::exit(1) if an error occurs. All the constructors accept an additional
///       option to turn this behaviour off, and throw a UserError instead.
/// @note Main can only be initialised once, so we need to generate a binary per test case via ifdefs

namespace {

/// A local function to satisfy the CmdArg details
void usage(const std::string&) {
    // Empty
}

void init(int nargs, const char* global_args[]) {
    Main::initialise(nargs, const_cast<char**>(global_args));
    CmdArgs(&usage, 1, 0, true);
}

void init(int nargs, const char* global_args[], std::vector<Option*>& options, int args_count = 0) {
    Main::initialise(nargs, const_cast<char**>(global_args));
    CmdArgs(&usage, options, args_count, 0, true);
}
}  // namespace

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE >= 1 and TESTCASE <= 3
CASE("test_eckit_option_cmdargs_numbered_args_required") {
    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

#if TESTCASE == 1
    const char* args1[] = {"exe"};
    EXPECT_THROWS_AS(init(1, args1), UserError);
#endif

#if TESTCASE == 2
    const char* args2[] = {"exe", "a1"};
    EXPECT_NO_THROW(init(2, args2));
#endif

#if TESTCASE == 3
    const char* args3[] = {"exe", "a1", "a2"};
    EXPECT_THROWS_AS(init(3, args3), UserError);
#endif
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE >= 4 and TESTCASE <= 6
CASE("test_eckit_option_cmdargs_numbered_args_required_with_options") {
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));

    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

#if TESTCASE == 4
    const char* args1[] = {"exe"};
    EXPECT_THROWS_AS(init(1, args1, options, 1), UserError);
#endif

#if TESTCASE == 5
    const char* args2[] = {"exe", "a1"};
    EXPECT_NO_THROW(init(2, args2, options, 1));
#endif

#if TESTCASE == 6
    const char* args3[] = {"exe", "a1", "a2"};
    EXPECT_THROWS_AS(init(3, args3, options, 1), UserError);
#endif
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 7
CASE("test_eckit_option_cmdargs_simple_argument_string") {
    // Set up he parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg1=testing"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg1"));
    EXPECT(!args.has("arg2"));

    std::string tmpstr;
    args.get("arg1", tmpstr);
    EXPECT(tmpstr == "testing");
    EXPECT(args.getString("arg1") == "testing");
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 8
CASE("test_eckit_option_cmdargs_simple_argument_integer") {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg2=12345"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg2"));
    EXPECT(!args.has("arg1"));

    long tmpi;
    args.get("arg2", tmpi);
    EXPECT(tmpi == 12345);
    EXPECT(args.getLong("arg2") == 12345);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 9
CASE("test_eckit_option_cmdargs_simple_argument_missing") {
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg3=12345"};
    EXPECT_THROWS_AS(init(2, input, options), UserError);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 10
CASE("test_eckit_option_cmdargs_integer_vector") {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<long>("arg", "", 3));

    const char* input[] = {"exe", "--arg=-12345/678/-123"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg"));

    std::vector<long> tmpv;
    args.get("arg", tmpv);
    EXPECT(tmpv.size() == 3);
    EXPECT(tmpv[0] == -12345);
    EXPECT(tmpv[1] == 678);
    EXPECT(tmpv[2] == -123);

    EXPECT(tmpv == args.getLongVector("arg"));
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 11
CASE("test_eckit_option_cmdargs_double_vector") {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<double>("arg", "", 4));

    const char* input[] = {"exe", "--arg=-123.45/67.8/90/-123.0"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg"));

    std::vector<double> tmpv;
    args.get("arg", tmpv);
    EXPECT(tmpv.size() == 4);
    EXPECT(is_approximately_equal(tmpv[0], -123.45, 1.0e-8));
    EXPECT(is_approximately_equal(tmpv[1], 67.8, 1.0e-8));
    EXPECT(is_approximately_equal(tmpv[2], static_cast<double>(90), 1.0e-8));
    EXPECT(is_approximately_equal(tmpv[3], static_cast<double>(-123), 1.0e-8));

    EXPECT(tmpv == args.getDoubleVector("arg"));
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 12
CASE("test_eckit_option_cmdargs_vector_size_check") {
    std::vector<Option*> options;
    options.push_back(new VectorOption<long>("arg", "", 4));

    const char* input[] = {"exe", "--arg=1/2/3"};
    EXPECT_THROWS_AS(init(2, input, options), UserError);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv, false);  // will not call main::initialize
}
