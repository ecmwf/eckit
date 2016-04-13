/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_option

#include "ecbuild/boost_test_framework.h"

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"
#include "eckit/runtime/Context.h"

#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace eckit;
using namespace eckit::option;

/// Test the options parser
/// @note The options parser normall calls ::exit(1) if an error occurs. All the constructors accept an additional
///       option to turn this behaviour off, and throw a UserError instead.


namespace {

    /// A local function to satisfy the CmdArg details
    void usage(const std::string& tool) {
        // Empty
    }
}


BOOST_AUTO_TEST_SUITE( test_eckit_option_cmdargs )

BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_numbered_args_required )
{
    // Functor needed to test constructor inside BOOST_CHECK_THROW
    struct functor {
        /// Set up argument parser to accept one unnamed argument.
        void operator() (size_t nargs, const char* global_args[]) {

            Context::instance().setup(nargs, const_cast<char**>(global_args));
            CmdArgs args(&usage, 1, true);

            BOOST_CHECK_EQUAL(args.args()[0], std::string("a1"));
        }
    };

    const char* args1[] = {"exe"};
    const char* args2[] = {"exe", "a1"};
    const char* args3[] = {"exe", "a1", "a2"};

    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

    BOOST_CHECK_THROW(functor()(1, args1), UserError);

    functor()(2, args2);

    BOOST_CHECK_THROW(functor()(3, args3), UserError);
}


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_numbered_args_required_with_options )
{
    // Functor needed to test constructor inside BOOST_CHECK_THROW
    struct functor {
        /// Set up argument parser to accept one unnamed argument.
        void operator() (size_t nargs, const char* global_args[]) {

            std::vector<Option*> options;
            options.push_back(new SimpleOption<std::string>("arg1", ""));

            Context::instance().setup(nargs, const_cast<char**>(global_args));
            CmdArgs args(&usage, 1, options, true);

            BOOST_CHECK_EQUAL(args.args()[0], std::string("a1"));
        }
    };

    const char* args1[] = {"exe"};
    const char* args2[] = {"exe", "a1"};
    const char* args3[] = {"exe", "a1", "a2"};

    // Argument parser will succeed when passed exactly one unnamed argument.
    // Note that argument 0 is always the executable name.

    BOOST_CHECK_THROW(functor()(1, args1), UserError);

    functor()(2, args2);

    BOOST_CHECK_THROW(functor()(3, args3), UserError);
}


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_keys )
{
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg1=testing"};
    Context::instance().setup(2, const_cast<char**>(input));

    CmdArgs args(&usage, 0, options, true);

    std::set<std::string> keys = args.keys();

    ASSERT(keys.find("arg1") != keys.end());
    ASSERT(keys.find("arg2") != keys.end());
}


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_string ) {

    // Set up he parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg1=testing"};
    Context::instance().setup(2, const_cast<char**>(input));

    CmdArgs args(&usage, 0, options, true);
    BOOST_CHECK(args.has("arg1"));
    BOOST_CHECK(!args.has("arg2"));

    std::string tmpstr;
    args.get("arg1", tmpstr);
    BOOST_CHECK_EQUAL(tmpstr, "testing");
    BOOST_CHECK_EQUAL(args.getString("arg1"), "testing");
}


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_integer ) {

    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    const char* input[] = {"exe", "--arg2=12345"};
    Context::instance().setup(2, const_cast<char**>(input));

    CmdArgs args(&usage, 0, options, true);
    BOOST_CHECK(args.has("arg2"));
    BOOST_CHECK(!args.has("arg1"));

    long tmpi;
    args.get("arg2", tmpi);
    BOOST_CHECK_EQUAL(tmpi, 12345);
    BOOST_CHECK_EQUAL(args.getLong("arg2"), 12345);
}

BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_simple_argument_missing ) {

    // Functor needed to test constructor inside BOOST_CHECK_THROW
    // Oh, wouldn't lambda functions be a glorious thing!
    struct functor {
        void operator() () {

            std::vector<Option*> options;
            options.push_back(new SimpleOption<std::string>("arg1", ""));
            options.push_back(new SimpleOption<long>("arg2", ""));

            const char* input[] = {"exe", "--arg3=12345"};
            Context::instance().setup(2, const_cast<char**>(input));
            CmdArgs args(&usage, 0, options, true);
        }
    };

    BOOST_CHECK_THROW(functor()(), UserError);
}


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_integer_vector ) {

    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<long>("arg", "", 3));

    const char* input[] = {"exe", "--arg=-12345/678/-123"};
    Context::instance().setup(2, const_cast<char**>(input));

    CmdArgs args(&usage, 0, options, true);
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


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_double_vector ) {

    // Set up the parser to accept two named arguments, one integer and one string
    // n.b. Option* are deleted inside CmdArgs.
    std::vector<Option*> options;
    options.push_back(new VectorOption<double>("arg", "", 4));

    const char* input[] = {"exe", "--arg=-123.45/67.8/90/-123.0"};
    Context::instance().setup(2, const_cast<char**>(input));

    CmdArgs args(&usage, 0, options, true);
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


BOOST_AUTO_TEST_CASE( test_eckit_option_cmdargs_vector_size_check ) {

    // Functor needed to test constructor inside BOOST_CHECK_THROW
    // Oh, wouldn't lambda functions be a glorious thing!
    struct functor {
        void operator() () {

            std::vector<Option*> options;
            options.push_back(new VectorOption<long>("arg", "", 4));

            const char* input[] = {"exe", "--arg=1/2/3"};
            Context::instance().setup(2, const_cast<char**>(input));
            CmdArgs args(&usage, 0, options, true);
        }
    };

    BOOST_CHECK_THROW(functor()(), UserError);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
