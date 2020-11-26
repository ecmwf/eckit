/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/parser/JSONParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Parse JSON to Value") {

    std::istringstream in(
        "{ \"a\" : [true, false, 3], \"b\" : 42.3 , \"c\" : null, \"d\" : \"y\n\tr\rh\", \"e\" : "
        "\"867017db84f4bc2b5078ca56ffd3b9b9\"}");
    JSONParser p(in);

    Value v = p.parse();

    //    Log::info() << "json " << v << std::endl;

    Log::info() << v << std::endl;
    Log::info() << v["a"] << std::endl;
    Log::info() << v["a"][2] << std::endl;

    JSON j(cout);
    j << v;

    EXPECT(v.isOrderedMap());
    EXPECT(v.as<ValueMap>().size() == 5);

    EXPECT(v["a"].isList());
    EXPECT(v["a"].as<ValueList>().size() == 3);


    EXPECT(v["a"][0].isBool());
    EXPECT(v["a"][0].as<bool>() == true);

    EXPECT(v["a"][1].isBool());
    EXPECT(v["a"][1].as<bool>() == false);

    EXPECT(v["a"][2].isNumber());
    EXPECT((int)v["a"][2] == 3);

    EXPECT(v["b"].isDouble());
    EXPECT(v["b"].as<double>() - 42.3 < 1E-12);

    EXPECT(v["c"].isNil());

    EXPECT(v["d"].isString());

    EXPECT(v["e"].isString());
    EXPECT(v["e"].as<string>() == "867017db84f4bc2b5078ca56ffd3b9b9");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_parse_to_set") {
    istringstream in("[ \"a\" , \"b\", \"c\" ]");
    JSONParser p(in);
    Value v = p.parse();

    //    Log::info() << "json " << v << std::endl;


    Log::info() << v << std::endl;

    EXPECT(v.isList());
    EXPECT(v.as<ValueList>().size() == 3);

    EXPECT(v[0].isString());
    EXPECT(v[0].as<string>() == "a");

    EXPECT(v[1].isString());
    EXPECT(v[1].as<string>() == "b");

    EXPECT(v[2].isString());
    EXPECT(v[2].as<string>() == "c");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_parse_to_map") {
    istringstream in("{ \"a\" : \"AAA\", \"b\" : 0.0 , \"c\" : \"null\", \"d\" : \"\"}");
    JSONParser p(in);
    Value v = p.parse();

    //    Log::info() << "json " << v << std::endl;


    Log::info() << v << std::endl;

    EXPECT(v.isOrderedMap());
    EXPECT(v.as<ValueMap>().size() == 4);

    EXPECT(v["a"].isString());
    EXPECT(v["a"].as<string>() == "AAA");

    EXPECT(v["b"].isDouble());
    EXPECT(v["b"].as<double>() == 0.0);

    EXPECT(v["c"].isString());
    EXPECT(v["c"].as<string>() == "null");

    EXPECT(v["d"].isString());
    EXPECT(v["d"].as<string>() == "");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_eof") {
    istringstream in("");
    JSONParser p(in);

    EXPECT_THROWS_AS(p.next(), StreamParser::Error);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_parser_comment_in_string") {
    istringstream in("{\"test\": \"#fff\"}");
    JSONParser p(in);

    Value v = p.parse();
    Log::info() << v << std::endl;

    EXPECT(v["test"] == "#fff");
}

#ifdef eckit_HAVE_UNICODE
CASE("test_eckit_parser_unicode") {
    istringstream in("{\"test\": \"\\u0061\"}");
    JSONParser p(in);

    Value v = p.parse();
    Log::info() << v << std::endl;

    EXPECT(v["test"] == "a");
}
#endif  // eckit_HAVE_UNICODE

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
