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
#include "eckit/option/MultiValueOption.h"
#include "eckit/option/Separator.h"
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

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

/// Test the options parser
/// @note The options parser normall calls ::exit(1) if an error occurs. All the constructors accept an additional
///       option to turn this behaviour off, and throw a UserError instead.
/// @note Main can only be initialised once, so we need to generate a binary per test case via ifdefs

namespace {

/// A self-cleaning container for Option
struct options_t : public std::vector<Option*> {
    using vector::push_back;

    ~options_t() {
        for (auto* option : *this) {
            delete option;
        }
    }
};

/// A local function to satisfy the CmdArg details
void usage(const std::string&) {
    // Empty
}

void init(int nargs, const char* global_args[]) {
    Main::initialise(nargs, const_cast<char**>(global_args));
    CmdArgs(&usage, 1, 0, true);
}

void init(int nargs, const char* global_args[], options_t& options, int args_count = 0) {
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
    options_t options;
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
    options_t options;
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
    options_t options;
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
    options_t options;
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
    options_t options;
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
    options_t options;
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
    options_t options;
    options.push_back(new VectorOption<long>("arg", "", 4));

    const char* input[] = {"exe", "--arg=1/2/3"};
    EXPECT_THROWS_AS(init(2, input, options), UserError);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if TESTCASE == 13
CASE("test_eckit_option__string_with_default_value") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", "", "default-value"));

    std::vector<const char*> input = {"exe"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]), nullptr);

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    EXPECT(args.has("arg1"));
    auto arg1_actual = args.getString("arg1");
    EXPECT_EQUAL(arg1_actual, "default-value");
}
#endif

#if TESTCASE == 14
CASE("test_eckit_option__string_no_default_value") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));

    std::vector<const char*> input = {"exe"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]), nullptr);

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    EXPECT(!args.has("arg1"));
}
#endif

#if TESTCASE == 15
CASE("test_eckit_option__bool_plus_separated_string") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<bool>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg2", "--arg1", "value"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]), nullptr);

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg1"));
    EXPECT(args.has("arg2"));
    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value");
    }
    {
        bool flag;
        args.get("arg2", flag);
        EXPECT_EQUAL(flag, true);
        auto arg2_actual = args.getBool("arg2");
        EXPECT_EQUAL(arg2_actual, true);
    }
}
#endif

#if TESTCASE == 16
CASE("test_eckit_option__bool_plus_joint_string") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<bool>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg2", "--arg1=value"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg1"));
    EXPECT(args.has("arg2"));
    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value");
    }
    {
        bool flag;
        args.get("arg2", flag);
        EXPECT_EQUAL(flag, true);
        auto arg2_actual = args.getBool("arg2");
        EXPECT_EQUAL(arg2_actual, true);
    }
}
#endif

#if TESTCASE == 17
CASE("test_eckit_option__long_plus_joint_string") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<long>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg2=12345", "--arg1=value"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);
    EXPECT(args.has("arg1"));
    EXPECT(args.has("arg2"));
    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value");
    }

    {
        long i;
        args.get("arg2", i);
        EXPECT_EQUAL(i, 12345);
        auto arg2_actual = args.getLong("arg2");
        EXPECT_EQUAL(arg2_actual, 12345L);
    }
}
#endif

#if TESTCASE == 18
CASE("test_eckit_option__long_plus_joint_and_separated_string") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<std::string>("arg2", ""));
    options.push_back(new SimpleOption<long>("arg3", ""));

    std::vector<const char*> input = {"exe",  "--arg1",        "value1", "pos1",        "pos2",
                                      "pos3", "--arg2=value2", "pos4",   "--arg3=1234", "pos5"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 4, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 5);
    auto pos1_actual = args(0);
    EXPECT_EQUAL(pos1_actual, "pos1");
    auto pos2_actual = args(1);
    EXPECT_EQUAL(pos2_actual, "pos2");
    auto pos3_actual = args(2);
    EXPECT_EQUAL(pos3_actual, "pos3");
    auto pos4_actual = args(3);
    EXPECT_EQUAL(pos4_actual, "pos4");
    auto pos5_actual = args(4);
    EXPECT_EQUAL(pos5_actual, "pos5");

    {
        EXPECT(args.has("arg1"));
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value1");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value1");
    }

    {
        EXPECT(args.has("arg2"));
        std::string s;
        args.get("arg2", s);
        EXPECT_EQUAL(s, "value2");
        auto arg2_actual = args.getString("arg2");
        EXPECT_EQUAL(arg2_actual, "value2");
    }

    {
        EXPECT(args.has("arg3"));
        long i;
        args.get("arg3", i);
        EXPECT_EQUAL(i, 1234L);
        auto arg3_actual = args.getLong("arg3");
        EXPECT_EQUAL(arg3_actual, 1234L);
    }
}
#endif

#if TESTCASE == 19
CASE("test_eckit_option__multi_separate_value_alone") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "value2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
    }
}
#endif

#if TESTCASE == 20
CASE("test_eckit_option__multi_joint_value_alone") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2));

    std::vector<const char*> input = {"exe", "--arg1=value1", "value2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
    }
}
#endif

#if TESTCASE == 21
CASE("test_eckit_option__multi_value_no_default_value") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2));

    std::vector<const char*> input = {"exe", "pos1", "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    EXPECT(!args.has("arg1"));
}
#endif

#if TESTCASE == 22
CASE("test_eckit_option__multi_value_with_default_value") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2, MultiValueOption::values_t{"value1", "value2"}));

    std::vector<const char*> input = {"exe", "pos1", "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
    }
}
#endif

#if TESTCASE == 23
CASE("test_eckit_option__multi_value_with_1_mandatory_0_optional") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 1, 0));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "pos1", "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 1);
            EXPECT_EQUAL(v[0], "value1");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 1);
            EXPECT_EQUAL(v[0], "value1");
        }
    }
}
#endif

#if TESTCASE == 24
CASE("test_eckit_option__multi_value_with_2_mandatory_0_optional") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2, 0));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "value2", "pos1", "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
        }
    }
}
#endif

#if TESTCASE == 25
CASE("test_eckit_option__multi_value_with_2_mandatory_2_optional_2_provided") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2, 2));
    options.push_back(new SimpleOption<bool>("arg2", ""));

    std::vector<const char*> input = {"exe",       "--arg1", "value1", "value2", "optional1",
                                      "optional2", "--arg2", "pos1",   "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 4);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
            EXPECT_EQUAL(v[3], "optional2");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 4);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
            EXPECT_EQUAL(v[3], "optional2");
        }
    }

    EXPECT(args.has("arg2"));
}
#endif

#if TESTCASE == 26
CASE("test_eckit_option__multi_value_with_2_mandatory_2_optional_1_provided_followed_by_another_option") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2, 2));
    options.push_back(new SimpleOption<bool>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "value2", "optional1", "--arg2", "pos1", "pos2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 3);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 3);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
        }
    }

    EXPECT(args.has("arg2"));
}
#endif

#if TESTCASE == 27
CASE("test_eckit_option__multi_value_with_2_mandatory_2_optional_1_provided_at_end") {
    options_t options;
    options.push_back(new MultiValueOption("arg1", "", 2, 2));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "value2", "optional1"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        {
            std::vector<std::string> v;
            args.get("arg1", v);
            EXPECT_EQUAL(v.size(), 3);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
        }
        {
            std::vector<std::string> v = args.getStringVector("arg1");
            EXPECT_EQUAL(v.size(), 3);
            EXPECT_EQUAL(v[0], "value1");
            EXPECT_EQUAL(v[1], "value2");
            EXPECT_EQUAL(v[2], "optional1");
        }
    }
}
#endif

#if TESTCASE == 28
CASE("test_eckit_option__multi_value_with_others") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new SimpleOption<std::string>("arg2", ""));
    options.push_back(new MultiValueOption("label", "", 2));

    std::vector<const char*> input = {"exe",
                                      "--arg1",
                                      "value1",
                                      "pos1",
                                      "pos2",
                                      "pos3",
                                      "--arg2=value2",
                                      "pos4",
                                      "--label=somelabel",
                                      "--a-label-value--",
                                      "pos5"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 4, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 5);
    auto pos1_actual = args(0);
    EXPECT_EQUAL(pos1_actual, "pos1");
    auto pos2_actual = args(1);
    EXPECT_EQUAL(pos2_actual, "pos2");
    auto pos3_actual = args(2);
    EXPECT_EQUAL(pos3_actual, "pos3");
    auto pos4_actual = args(3);
    EXPECT_EQUAL(pos4_actual, "pos4");
    auto pos5_actual = args(4);
    EXPECT_EQUAL(pos5_actual, "pos5");

    {
        EXPECT(args.has("arg1"));
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value1");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value1");
    }

    {
        EXPECT(args.has("arg2"));
        std::string s;
        args.get("arg2", s);
        EXPECT_EQUAL(s, "value2");
        auto arg2_actual = args.getString("arg2");
        EXPECT_EQUAL(arg2_actual, "value2");
    }

    {
        EXPECT(args.has("label"));
        {
            std::vector<std::string> v;
            args.get("label", v);
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "somelabel");
            EXPECT_EQUAL(v[1], "--a-label-value--");
        }
        {
            std::vector<std::string> v = args.getStringVector("label");
            EXPECT_EQUAL(v.size(), 2);
            EXPECT_EQUAL(v[0], "somelabel");
            EXPECT_EQUAL(v[1], "--a-label-value--");
        }
    }
}
#endif

#if TESTCASE == 29
CASE("test_eckit_option__with_separator") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new Separator("Separator"));
    options.push_back(new SimpleOption<std::string>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "--arg2=value2"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "value1");
        auto arg1_actual = args.getString("arg1");
        EXPECT_EQUAL(arg1_actual, "value1");
    }

    {
        EXPECT(args.has("arg2"));
        std::string s;
        args.get("arg2", s);
        EXPECT_EQUAL(s, "value2");
        auto arg2_actual = args.getString("arg2");
        EXPECT_EQUAL(arg2_actual, "value2");
    }
}
#endif

#if TESTCASE == 30
CASE("test_eckit_option__with_separator_and_some_invalid_option_to_force_logging_usage") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", ""));
    options.push_back(new Separator("Separator"));
    options.push_back(new SimpleOption<std::string>("arg2", ""));

    std::vector<const char*> input = {"exe", "--arg1", "value1", "--arg2=value2", "--invalid=!!!"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    try {
        CmdArgs args(&usage, options, 0, 0, true);
        EXPECT(false);
    }
    catch (const Exception& e) {
        EXPECT(std::string(e.what()).find("UserError: An error occurred in argument parsing") != std::string::npos);
    }
}
#endif

#if TESTCASE == 31
CASE("test_eckit_option__with_simple_string_value_containing_equals_sign") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", "description"));

    std::vector<const char*> input = {"exe", "--arg1=some=value=with=equals", "/some/path", "/another/path"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, -1, 2, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 2);

    {
        EXPECT(args.has("arg1"));
        std::string s;
        args.get("arg1", s);
        EXPECT_EQUAL(s, "some=value=with=equals");
    }
}
#endif

#if TESTCASE == 32
CASE("test_eckit_option__can_handle_bool_option_without_value") {
    options_t options;
    options.push_back(new SimpleOption<bool>("arg1", "description"));

    std::vector<const char*> input = {
        "exe",
        "--arg1",
    };
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        bool v = args.getBool("arg1");
        EXPECT_EQUAL(v, true);
    }
}
#endif

#if TESTCASE == 33
CASE("test_eckit_option__can_handle_bool_option_with_true_value") {
    options_t options;
    options.push_back(new SimpleOption<bool>("arg1", "description"));

    std::vector<const char*> input = {
        "exe",
        "--arg1=yes",
    };
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        bool v = args.getBool("arg1");
        EXPECT_EQUAL(v, true);
    }
}
#endif

#if TESTCASE == 34
CASE("test_eckit_option__can_handle_bool_option_with_false_value") {
    options_t options;
    options.push_back(new SimpleOption<bool>("arg1", "description"));

    std::vector<const char*> input = {
        "exe",
        "--arg1=0",
    };
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    auto args_count = args.count();
    EXPECT_EQUAL(args_count, 0);

    {
        EXPECT(args.has("arg1"));
        bool v = args.getBool("arg1");
        EXPECT_EQUAL(v, false);
    }
}
#endif

#if TESTCASE == 35
CASE("test_eckit_option__can_handle_simple_option_provided_twice") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("arg1", "description"));

    std::vector<const char*> input = {
        "exe", "--arg1=first.value", "--arg1", "second.value", "--arg1=last.value",
    };
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    {
        EXPECT(args.has("arg1"));
        std::string s = args.getString("arg1");
        EXPECT_EQUAL(s, "last.value");
    }
}
#endif

#if TESTCASE == 36
CASE("test_eckit_option__can_handle_vector_option_of_double") {
    options_t options;
    options.push_back(new VectorOption<double>("arg1", "description", 2));

    std::vector<const char*> input = {"exe", "--arg1=-32.0/10"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    {
        EXPECT(args.has("arg1"));
        auto s = args.getDoubleVector("arg1");
        EXPECT_EQUAL(s.size(), 2);
    }
}
#endif

#if TESTCASE == 37
CASE("test_eckit_option_cmdargs_value_with_equals") {
    options_t options;
    for (auto c : std::string{"abcdefghijk"}) {
        options.push_back(new SimpleOption<std::string>(std::string{c}, ""));
    }

    std::vector<const char*> input = {"exe",     "--a=a",   "--b==b",   "--c=c=",   "--d===d",  "--e==e=",
                                      "--f=f==", "--g=g=g", "--h==h=h", "--i==ii=", "--j=j==j", "--k=k=k="};

    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    EXPECT(args.getString("a") == "a");
    EXPECT(args.getString("b") == "=b");
    EXPECT(args.getString("c") == "c=");
    EXPECT(args.getString("d") == "==d");
    EXPECT(args.getString("e") == "=e=");
    EXPECT(args.getString("f") == "f==");
    EXPECT(args.getString("g") == "g=g");
    EXPECT(args.getString("h") == "=h=h");
    EXPECT(args.getString("i") == "=ii=");
    EXPECT(args.getString("j") == "j==j");
    EXPECT(args.getString("k") == "k=k=");
}
#endif

#if TESTCASE == 38
// disable deprecated declarations warning here as we are testing it
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__INTEL_COMPILER)
#pragma warning ( push )
#pragma warning ( disable:1786 )
#elif defined(__NVCOMPILER)
#pragma diag_suppress 1445
#endif
CASE("test_eckit_option__allows_to_set_default_value_for_options") {
    options_t options;
    {
        auto o = (new SimpleOption<std::string>("arg1", ""))->defaultValue("default string");
        options.push_back(o);
    }
    {
        auto o = (new VectorOption<std::string>("arg2", "", 0))->defaultValue("q/w/e/r/t/y");
        options.push_back(o);
    }
    {
        auto o = (new VectorOption<long>("arg3", "", 0))->defaultValue("1/2/3/4");
        options.push_back(o);
    }

    std::vector<const char*> input = {"exe"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);

    {
        EXPECT(args.has("arg1"));
        auto v = args.getString("arg1");
        EXPECT_EQUAL(v, "default string");
    }
    {
        EXPECT(args.has("arg2"));
        auto v = args.getStringVector("arg2");
        EXPECT_EQUAL(v.size(), 6);
        EXPECT_EQUAL(v[0], "q");
        EXPECT_EQUAL(v[1], "w");
        EXPECT_EQUAL(v[2], "e");
        EXPECT_EQUAL(v[3], "r");
        EXPECT_EQUAL(v[4], "t");
        EXPECT_EQUAL(v[5], "y");
    }
    {
        EXPECT(args.has("arg3"));
        auto v = args.getLongVector("arg3");
        EXPECT_EQUAL(v.size(), 4);
        EXPECT_EQUAL(v[0], 1L);
        EXPECT_EQUAL(v[1], 2L);
        EXPECT_EQUAL(v[2], 3L);
        EXPECT_EQUAL(v[3], 4L);
    }
}
#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__INTEL_COMPILER)
#pragma warning ( pop )
#endif
#endif

#if TESTCASE == 39
CASE("test_eckit_option__cmdargs_argument_with_dot_in_option_name") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("a.a", ""));
    options.push_back(new SimpleOption<std::string>("b.b.b", ""));
    options.push_back(new SimpleOption<std::string>("c.", ""));
    options.push_back(new SimpleOption<std::string>(".d", ""));

    std::vector<const char*> input = {"exe", "--a.a=a", "--b.b.b=b", "--c.=c", "--.d=d"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);
    Log::info() << args << std::endl;

    std::string a;
    EXPECT(args.get("a.a", a));
    EXPECT(a == "a");
    EXPECT(args.getString("a.a") == a);

    std::string b;
    EXPECT(args.get("b.b.b", b));
    EXPECT(b == "b");
    EXPECT(args.getString("b.b.b") == b);

    std::string c;
    EXPECT(args.get("c.", c));
    EXPECT(c == "c");
    EXPECT(args.getString("c.") == c);

    std::string d;
    EXPECT(args.get(".d", d));
    EXPECT(d == "d");
    EXPECT(args.getString(".d") == d);
}
#endif

#if TESTCASE == 40
CASE("test_eckit_option__cmdargs_argument_with_dot_in_option_name_and_value") {
    options_t options;
    options.push_back(new SimpleOption<std::string>("a.a", ""));
    options.push_back(new SimpleOption<std::string>("b.b.b", ""));
    options.push_back(new SimpleOption<std::string>("c.", ""));
    options.push_back(new SimpleOption<std::string>(".d", ""));

    std::vector<const char*> input = {"exe", "--a.a=a.a", "--b.b.b=b.b.b", "--c.=c.", "--.d=.d"};
    Main::initialise(input.size(), const_cast<char**>(&input[0]));

    CmdArgs args(&usage, options, 0, 0, true);
    Log::info() << args << std::endl;

    std::string a;
    EXPECT(args.get("a.a", a));
    EXPECT(a == "a.a");
    EXPECT(args.getString("a.a") == a);

    std::string b;
    EXPECT(args.get("b.b.b", b));
    EXPECT(b == "b.b.b");
    EXPECT(args.getString("b.b.b") == b);

    std::string c;
    EXPECT(args.get("c.", c));
    EXPECT(c == "c.");
    EXPECT(args.getString("c.") == c);

    std::string d;
    EXPECT(args.get(".d", d));
    EXPECT(d == ".d");
    EXPECT(args.getString(".d") == d);
}
#endif


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv, false);  // will not call main::initialize
}
