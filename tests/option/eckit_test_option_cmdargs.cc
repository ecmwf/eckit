/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_option

#include "ecbuild/boost_test_framework.h"

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"
#include "eckit/runtime/Main.h"

#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace eckit;
using namespace eckit::option;

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
}

BOOST_AUTO_TEST_SUITE( test_eckit_option_cmdargs )

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE >= 1 and TESTCASE <= 3
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_numbered_args_required ) {
    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

#if TESTCASE == 1
    const char* args1[] = {"exe"};
    BOOST_CHECK_THROW(init(1, args1), UserError);
#endif

#if TESTCASE == 2
    const char* args2[] = {"exe", "a1"};
    BOOST_CHECK_NO_THROW(init(2, args2));
#endif

#if TESTCASE == 3
    const char* args3[] = {"exe", "a1", "a2"};
    BOOST_CHECK_THROW(init(3, args3), UserError);
#endif
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE >= 4 and TESTCASE <= 6
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_numbered_args_required_with_options ) {
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));

    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

#if TESTCASE == 4
    const char* args1[] = {"exe"};
    BOOST_CHECK_THROW(init(1, args1, options, 1), UserError);
#endif

#if TESTCASE == 5
    const char* args2[] = {"exe", "a1"};
    BOOST_CHECK_NO_THROW(init(2, args2, options, 1));
#endif

#if TESTCASE == 6
    const char* args3[] = {"exe", "a1", "a2"};
    BOOST_CHECK_THROW(init(3, args3, options, 1), UserError);
#endif
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 7
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_string ) {
    // Set up he parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg1=testing"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    BOOST_CHECK(args.has("arg1"));
    BOOST_CHECK(!args.has("arg2"));

    std::string tmpstr;
    args.get("arg1", tmpstr);
    BOOST_CHECK_EQUAL(tmpstr, "testing");
    BOOST_CHECK_EQUAL(args.getString("arg1"), "testing");
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 8
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_integer ) {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg2=12345"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    BOOST_CHECK(args.has("arg2"));
    BOOST_CHECK(!args.has("arg1"));

    long tmpi;
    args.get("arg2", tmpi);
    BOOST_CHECK_EQUAL(tmpi, 12345);
    BOOST_CHECK_EQUAL(args.getLong("arg2"), 12345);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 9
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_missing ) {
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg3=12345"};
    BOOST_CHECK_THROW(init(2, input, options), UserError);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 10
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_integer_vector ) {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<long>("arg", "", 3));

    const char* input[] = {"exe", "--arg=-12345/678/-123"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    BOOST_CHECK(args.has("arg"));

    std::vector<long> tmpv;
    args.get("arg", tmpv);
    BOOST_CHECK_EQUAL(tmpv.size(), 3);
    BOOST_CHECK_EQUAL(tmpv[0], -12345);
    BOOST_CHECK_EQUAL(tmpv[1], 678);
    BOOST_CHECK_EQUAL(tmpv[2], -123);

    // Check equality directly to avoid exciting operator<< gubbins within BOOST_CHECK_EQUAL.
    BOOST_CHECK(tmpv == args.getLongVector("arg"));
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 11
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_double_vector ) {
    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<double>("arg", "", 4));

    const char* input[] = {"exe", "--arg=-123.45/67.8/90/-123.0"};
    Main::initialise(2, const_cast<char**>(input));

    CmdArgs args(&usage, options, 0, 0, true);
    BOOST_CHECK(args.has("arg"));

    std::vector<double> tmpv;
    args.get("arg", tmpv);
    BOOST_CHECK_EQUAL(tmpv.size(), 4);
    BOOST_CHECK_CLOSE(tmpv[0], -123.45, 1.0e-8);
    BOOST_CHECK_CLOSE(tmpv[1], 67.8, 1.0e-8);
    BOOST_CHECK_CLOSE(tmpv[2], 90, 1.0e-8);
    BOOST_CHECK_CLOSE(tmpv[3], -123, 1.0e-8);

    // Check equality directly to avoid exciting operator<< gubbins within BOOST_CHECK_EQUAL.
    BOOST_CHECK(tmpv == args.getDoubleVector("arg"));
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 12
BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_vector_size_check ) {
    std::vector<Option*> options;
    options.push_back(new VectorOption<long>("arg", "", 4));

    const char* input[] = {"exe", "--arg=1/2/3"};
    BOOST_CHECK_THROW(init(2, input, options), UserError);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
