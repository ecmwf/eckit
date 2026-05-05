/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"
#include "eckit/utils/StringTools.h"

#include <list>
#include <set>
#include <sstream>

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Convert to lower") {

    std::string istr = "Foo Bar";

    std::string ostr = StringTools::lower(istr);

    EXPECT(ostr == "foo bar");
}

CASE("Convert to upper") {

    std::string istr = "Foo Bar";

    std::string ostr = StringTools::upper(istr);

    EXPECT(ostr == "FOO BAR");
}

CASE("substitute") {
    StringDict m;

    m["class"]  = "od";
    m["stream"] = "oper";

    string in("{class}:none:{stream}");

    string out = StringTools::substitute(in, m);

    EXPECT(out == "od:none:oper");
}

CASE("listVariables") {
    string in("{class}:none:{stream}");

    StringList out = StringTools::listVariables(in);

    EXPECT(out.size() == 2);
    EXPECT(out[0] == "class");
    EXPECT(out[1] == "stream");
}

CASE("startsWith") {
    string in("_lolo_test");
    string s1("_lolo");
    string s2("lolo");
    string s3("_lolo_test_bigger");

    EXPECT(!StringTools::startsWith(in, ""));
    EXPECT(StringTools::startsWith(in, in));
    EXPECT(StringTools::startsWith(in, s1));
    EXPECT(StringTools::startsWith(in, "_"));
    EXPECT(!StringTools::startsWith(in, s2));
    EXPECT(!StringTools::startsWith(in, s3));
}

CASE("endsWith") {
    string in("_lolo_test");
    EXPECT(!StringTools::endsWith(in, ""));
    EXPECT(StringTools::endsWith(in, in));
    EXPECT(StringTools::endsWith(in, "t"));

    string s1("_test");
    EXPECT(StringTools::endsWith(in, s1));

    string s2("lolo");
    EXPECT(!StringTools::endsWith(in, s2));

    string s3("_lolo_test333");
    EXPECT(!StringTools::endsWith(in, s3));
}

CASE("trim") {
    string t1("   lolo_test    ");
    EXPECT(StringTools::trim(t1) == string("lolo_test"));

    string t2("   lolo_test");
    EXPECT(StringTools::trim(t2) == string("lolo_test"));

    string t3("lolo_test   ");
    EXPECT(StringTools::trim(t3) == string("lolo_test"));

    string t4("");
    EXPECT(StringTools::trim(t4) == string(""));

    string t5("nothing_here");
    EXPECT(StringTools::trim(t5) == string("nothing_here"));

    string t6("XXXXXXusefullXXXXX");
    EXPECT(StringTools::trim(t6, "X") == string("usefull"));

    string t7("0000010");
    EXPECT(StringTools::trim(t7, "0") == string("1"));
}

CASE("front_trim") {
    string t1("   lolo_test    ");
    EXPECT(StringTools::front_trim(t1) == string("lolo_test    "));

    string t2("   lolo_test");
    EXPECT(StringTools::front_trim(t2) == string("lolo_test"));

    string t3("lolo_test   ");
    EXPECT(StringTools::front_trim(t3) == string("lolo_test   "));

    string t4("");
    EXPECT(StringTools::front_trim(t4) == string(""));

    string t5("nothing_here");
    EXPECT(StringTools::front_trim(t5) == string("nothing_here"));

    string t6("XXXXXXusefullXXXXX");
    EXPECT(StringTools::front_trim(t6, "X") == string("usefullXXXXX"));

    string t7("0000010");
    EXPECT(StringTools::front_trim(t7, "0") == string("10"));
}

CASE("back_trim") {
    string t1("   lolo_test    ");

    Log::info() << StringTools::back_trim(t1) << std::endl;

    EXPECT(StringTools::back_trim(t1) == string("   lolo_test"));

    string t2("   lolo_test");
    EXPECT(StringTools::back_trim(t2) == string("   lolo_test"));

    string t3("lolo_test   ");
    EXPECT(StringTools::back_trim(t3) == string("lolo_test"));

    string t4("");
    EXPECT(StringTools::back_trim(t4) == string(""));

    string t5("nothing_here");
    EXPECT(StringTools::back_trim(t5) == string("nothing_here"));

    string t6("XXXXXXusefullXXXXX");
    EXPECT(StringTools::back_trim(t6, "X") == string("XXXXXXusefull"));

    string t7("0000010");
    EXPECT(StringTools::back_trim(t7, "0") == string("000001"));
}

CASE("joinOstream streams a delimited sequence with no temporary string") {
    std::vector<std::string> v{"alpha", "beta", "gamma"};

    std::ostringstream oss;
    oss << StringTools::joinOstream(v, ", ");
    EXPECT_EQUAL(oss.str(), "alpha, beta, gamma");
}

CASE("joinOstream handles edge cases (empty / single-element)") {
    std::vector<std::string> empty;
    std::ostringstream oss;
    oss << "[" << StringTools::joinOstream(empty, ", ") << "]";
    EXPECT_EQUAL(oss.str(), "[]");

    std::vector<std::string> one{"only"};
    oss.str("");
    oss << StringTools::joinOstream(one, ", ");
    EXPECT_EQUAL(oss.str(), "only");
}

CASE("joinOstream is generic over container and element type") {
    // std::list of strings
    std::list<std::string> l{"a", "b", "c"};
    std::ostringstream oss;
    oss << StringTools::joinOstream(l, "|");
    EXPECT_EQUAL(oss.str(), "a|b|c");

    // std::set: deterministic alphabetic order
    std::set<std::string> s{"gamma", "alpha", "beta"};
    oss.str("");
    oss << StringTools::joinOstream(s, "-");
    EXPECT_EQUAL(oss.str(), "alpha-beta-gamma");

    // Numeric vector: relies on operator<< for the element type, not std::string conversion.
    std::vector<int> nums{1, 2, 3, 4};
    oss.str("");
    oss << StringTools::joinOstream(nums, ", ");
    EXPECT_EQUAL(oss.str(), "1, 2, 3, 4");
}

CASE("joinOstream accepts string_view-compatible delimiters") {
    std::vector<std::string> v{"x", "y"};

    std::ostringstream oss;
    oss << StringTools::joinOstream(v, "::");  // const char*
    EXPECT_EQUAL(oss.str(), "x::y");

    const std::string sep = " -> ";
    oss.str("");
    oss << StringTools::joinOstream(v, sep);  // std::string -> string_view
    EXPECT_EQUAL(oss.str(), "x -> y");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
