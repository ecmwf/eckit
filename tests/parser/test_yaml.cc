/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/YAMLParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static std::string toJSON(const Value& v) {
    std::ostringstream oss;
    JSON json(oss);
    json << v;
    return oss.str();
}

// CASE ( "test_eckit_yaml_1" ) {
//     Value v =  YAMLParser::decodeFile("2.1.yaml");
//     std::cout << "2.1.yaml " << v << std::endl;
//     std::cout << toJSON(v) << std::endl;

//     EXPECT( v.isList() );
//     EXPECT( v.size() == 3 );

//     EXPECT( v[0] == "Mark McGwire" );
//     EXPECT( v[1] == "Sammy Sosa" );
//     EXPECT( v[2] == "Ken Griffey" );

//     // EXPECT(toJSON(v) == "[\"Mark McGwire\",\"Sammy Sosa\",\"Ken Griffey\"]");

// }

CASE("test_eckit_yaml_2") {
    Value v = YAMLParser::decodeFile("2.2.yaml");
    std::cout << "2.2.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    EXPECT(v.isOrderedMap());
    EXPECT(v.keys().size() == 3);

    EXPECT(v["hr"] == Value(65));
    EXPECT(v["avg"] == Value(0.278));
    EXPECT(v["rbi"] == Value(147));
}

CASE("test_eckit_yaml_3") {
    Value v = YAMLParser::decodeFile("2.3.yaml");
    std::cout << "2.3.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    EXPECT(v.isOrderedMap());
    EXPECT(v.keys().size() == 2);

    EXPECT(v["american"].isList());
    EXPECT(v["american"].size() == 3);

    EXPECT(v["national"].isList());
    EXPECT(v["national"].size() == 3);
}

CASE("test_eckit_yaml_4") {
    Value v = YAMLParser::decodeFile("2.4.yaml");
    std::cout << "2.4.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    EXPECT(v.isList());
    EXPECT(v.size() == 2);

    EXPECT(v[0].isOrderedMap());
    EXPECT(v[0].keys().size() == 3);

    EXPECT(v[1].isOrderedMap());
    EXPECT(v[1].keys().size() == 3);
}

CASE("test_eckit_yaml_5") {
    Value v = YAMLParser::decodeFile("2.5.yaml");
    std::cout << "2.5.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    EXPECT(v.isList());
    EXPECT(v.size() == 3);

    EXPECT(v[0].isList());
    EXPECT(v[0].size() == 3);

    EXPECT(v[1].isList());
    EXPECT(v[1].size() == 3);


    EXPECT(v[2].isList());
    EXPECT(v[2].size() == 3);
}

CASE("test_eckit_yaml_6") {
    Value v = YAMLParser::decodeFile("2.6.yaml");
    std::cout << "2.6.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    EXPECT(v.isOrderedMap());
    EXPECT(v.keys().size() == 2);

    EXPECT(v["Mark McGwire"].isOrderedMap());
    EXPECT(v["Mark McGwire"].keys().size() == 2);

    EXPECT(v["Sammy Sosa"].isOrderedMap());
    EXPECT(v["Sammy Sosa"].keys().size() == 2);
}

CASE("test_eckit_yaml_7") {
    Value v = YAMLParser::decodeFile("2.7.yaml");
    std::cout << "2.7.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

CASE("test_eckit_yaml_8") {
    Value v = YAMLParser::decodeFile("2.8.yaml");
    std::cout << "2.8.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

CASE("test_eckit_yaml_9") {
    Value v = YAMLParser::decodeFile("2.9.yaml");
    std::cout << "2.9.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

CASE("test_eckit_yaml_10") {
    Value v = YAMLParser::decodeFile("2.10.yaml");
    std::cout << "2.10.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

/*
CASE ( "test_eckit_yaml_11" ) {
    Value v =  YAMLParser::decodeFile("2.11.yaml");
    std::cout << "2.11.yaml " << v << std::endl;
        std::cout << toJSON(v) << std::endl;
            std::cout << toJSON(v) << std::endl;

}
*/

CASE("test_eckit_yaml_12") {
    Value v = YAMLParser::decodeFile("2.12.yaml");
    std::cout << "2.12.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}
/*
CASE ( "test_eckit_yaml_13" ) {
    Value v =  YAMLParser::decodeFile("2.13.yaml");
    std::cout << "2.13.yaml " << v << std::endl;
        std::cout << toJSON(v) << std::endl;
}
*/

CASE("test_eckit_yaml_14") {
    Value v = YAMLParser::decodeFile("2.14.yaml");
    std::cout << "2.14.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    std::cout << "----------" << std::endl;
    std::cout << v << std::endl;
    std::cout << "----------" << std::endl;

    EXPECT(v == "Mark McGwire's year was crippled by a knee injury.");
}

CASE("test_eckit_yaml_15") {
    Value v = YAMLParser::decodeFile("2.15.yaml");
    std::cout << "2.15.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    std::cout << "----------" << std::endl;
    std::cout << v << std::endl;
    std::cout << "----------" << std::endl;

    EXPECT(v ==
           "Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting "
           "Average\n\nWhat a year!\n");
}

CASE("test_eckit_yaml_16") {
    Value v = YAMLParser::decodeFile("2.16.yaml");
    std::cout << "2.16.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

// CASE ( "test_eckit_yaml_17" ) {
//     Value v =  YAMLParser::decodeFile("2.17.yaml");
//     std::cout << "2.17.yaml " << v << std::endl;
//         std::cout << toJSON(v) << std::endl;
// }

CASE("test_eckit_yaml_18") {
    Value v = YAMLParser::decodeFile("2.18.yaml");
    std::cout << "2.18.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

CASE("test_eckit_yaml_19") {
    Value v = YAMLParser::decodeFile("2.19.yaml");
    std::cout << "2.19.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;


    EXPECT(v["canonical"] == Value(12345));
    // EXPECT(v["decimal"] == Value(12345));
    // EXPECT(v["sexagesimal"] == Value(12345));
    EXPECT(v["octal"] == Value(12));
    EXPECT(v["hexadecimal"] == Value(12));


    // canonical: 12345
    // decimal: +12,345
    // sexagesimal: 3:25:45
    // octal: 014
    // hexadecimal: 0xC
}

CASE("test_eckit_yaml_20") {
    Value v = YAMLParser::decodeFile("2.20.yaml");
    std::cout << "2.20.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


CASE("test_eckit_yaml_21") {
    Value v = YAMLParser::decodeFile("2.21.yaml");
    std::cout << "2.21.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

CASE("test_eckit_yaml_21_extra") {

    // Associated with ECKIT-260:
    // Case test_eckit_yaml_21 caused problems, because "true"/"false" is interpreted as a bool (rather than string) and
    // comparators were broken. Here we check that these keys are interpreted and compared correctly.

    // null: ~
    // true: y
    // false: n
    // string: '12345'

    // Check decoding of true, false, null, etc.
    EXPECT(YAMLParser::decodeString("null").typeName() == "Nil");
    EXPECT(YAMLParser::decodeString("false").typeName() == "Bool");
    EXPECT(YAMLParser::decodeString("~").typeName() == "String");
    EXPECT(YAMLParser::decodeString("y").typeName() == "String");
    EXPECT(YAMLParser::decodeString("n").typeName() == "String");
    EXPECT(YAMLParser::decodeString("string").typeName() == "String");
    EXPECT(YAMLParser::decodeString("'12345'").typeName() == "String");

    EXPECT(YAMLParser::decodeString("null") == Value());
    EXPECT(YAMLParser::decodeString("false") == Value(false));

    EXPECT(YAMLParser::decodeString("true").typeName() == "Bool");
    EXPECT(Value(true).typeName() == "Bool");
    EXPECT(Value(true) == Value(true));
    EXPECT(YAMLParser::decodeString("true") == YAMLParser::decodeString("true"));
    EXPECT(YAMLParser::decodeString("true") == Value(true));

    EXPECT(YAMLParser::decodeString("false").typeName() == "Bool");
    EXPECT(Value(false).typeName() == "Bool");
    EXPECT(Value(false) == Value(false));
    EXPECT(YAMLParser::decodeString("false") == YAMLParser::decodeString("false"));
    EXPECT(YAMLParser::decodeString("false") == Value(false));

    EXPECT(Value(false) != Value(true));
    EXPECT(YAMLParser::decodeString("false") != YAMLParser::decodeString("true"));
    EXPECT(YAMLParser::decodeString("false") != Value(true));

    EXPECT(Value(true) != Value(false));
    EXPECT(YAMLParser::decodeString("true") != YAMLParser::decodeString("false"));
    EXPECT(YAMLParser::decodeString("true") != Value(false));

    // Check ordering and comparisons
    EXPECT(Value() == Value());
    EXPECT(Value() < Value(true));
    EXPECT(Value() < Value(false));
    EXPECT(Value(false) == Value(false));
    EXPECT(Value(true) == Value(true));
    EXPECT(Value(false) < Value(true));
    EXPECT(!(Value(true) < Value(true)));
    EXPECT(!(Value(false) < Value(false)));
    EXPECT(Value("Something") == Value("Something"));
}

CASE("test_eckit_yaml_22") {
    Value v = YAMLParser::decodeFile("2.22.yaml");
    std::cout << "2.22.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


// CASE ( "test_eckit_yaml_23" ) {
//     Value v =  YAMLParser::decodeFile("2.23.yaml");
//     std::cout << "2.23.yaml " << v << std::endl;
// std::cout << toJSON(v) << std::endl;
// }


// CASE ( "test_eckit_yaml_24" ) {
//     Value v =  YAMLParser::decodeFile("2.24.yaml");
//     std::cout << "2.24.yaml " << v << std::endl;
// std::cout << toJSON(v) << std::endl;
// }


CASE("test_eckit_yaml_25") {
    Value v = YAMLParser::decodeFile("2.25.yaml");
    std::cout << "2.25.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


CASE("test_eckit_yaml_26") {
    Value v = YAMLParser::decodeFile("2.26.yaml");
    std::cout << "2.26.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


CASE("test_eckit_yaml_27") {
    Value v = YAMLParser::decodeFile("2.27.yaml");
    std::cout << "2.27.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


// CASE ( "test_eckit_yaml_28" ) {
//     Value v =  YAMLParser::decodeFile("2.28.yaml");
//     std::cout << "2.28.yaml " << v << std::endl;
//     std::cout << toJSON(v) << std::endl;
// }

CASE("test_eckit_yaml_cfg_1") {
    Value v = YAMLParser::decodeFile("cfg.1.yaml");
    std::cout << "cfg.1.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

/// @todo FIX this test

// CASE ( "test_eckit_yaml_key_with_space" ) {
//     Value v =  YAMLParser::decodeString("foo : bar");
//     EXPECT( v.contains("foo") );
//     EXPECT( v["foo"] == Value("bar") );
//}


#if __cplusplus > 199711L

CASE("test_eckit_ymal_text_1") {

    const char* text = R"YAML(
---
base: &base
    name: shared1
    address: shared2

foo: &foo
    <<: *base
    age: 10

bar: &bar
    <<: *base
    age: 20
)YAML";

    Value v = YAMLParser::decodeString(text);
    v.dump(std::cout) << std::endl;
}


CASE("test_eckit_yaml_text_2") {

    const char* text = R"YAML(
---
1: 2
)YAML";

    Value v = YAMLParser::decodeString(text);
    v.dump(std::cout) << std::endl;


    ValueMap m(v);

    EXPECT(v.keys()[0].isNumber());
}


CASE("test_eckit_yaml_text_3") {

    const char* text = R"YAML(
---
- '1'
- '2'
)YAML";

    Value v = YAMLParser::decodeString(text);
    v.dump(std::cout) << std::endl;
}


CASE("test_eckit_yaml_text_4") {

    const char* text = R"YAML(
---
165:
- 10u
- 10 metre u wind component
)YAML";

    Value v = YAMLParser::decodeString(text);
    v.dump(std::cout) << std::endl;
}


CASE("test_eckit_yaml_text_5") {

    {
        const char* text = "foo : bar";

        Value v = YAMLParser::decodeString(text);
        v.dump(std::cout) << std::endl;
        EXPECT(v["foo"] == Value("bar"));
    }

    // {
    //     const char* text = "'foo ': bar";

    //     Value v =  YAMLParser::decodeString(text);
    //     v.dump(std::cout) << std::endl;
    //     EXPECT(v["foo "] == Value("bar"));
    // }

    // {
    //     const char* text = "'foo ' : bar";

    //     Value v =  YAMLParser::decodeString(text);
    //     v.dump(std::cout) << std::endl;
    //     EXPECT(v["foo "] == Value("bar"));
    // }
}

CASE("test_eckit_yaml_text_6") {

    const char* text = R"YAML(
---
foo:
    bar: 1
  spam: true
)YAML";

    EXPECT_THROWS_AS(YAMLParser::decodeString(text), eckit::SeriousBug);
}

#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
