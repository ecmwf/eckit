/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <sstream>

#include "eckit/eckit_config.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/testing/Test.h"


namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

std::string to_json(const Value& v) {
    std::ostringstream oss;
    JSON json(oss);
    json << v;
    return oss.str();
}


CASE("test_eckit_yaml_11") {
    Value v = YAMLParser::decodeString(R"(
? - Detroit Tigers
  - Chicago cubs
:
  - 2001-07-23

? [ New York Yankees,
    Atlanta Braves ]
: [ 2001-07-02, 2001-08-12,
    2001-08-14 ]
)");
    std::cout << to_json(v) << std::endl;
}


CASE("test_eckit_yaml_16") {
    Value v = YAMLParser::decodeString(R"(
name: Mark McGwire
accomplishment: >
  Mark set a major league
  home run record in 1998.
stats: |
  65 Home Runs
  0.278 Batting Average

)");
    std::cout << to_json(v) << std::endl;
    EXPECT(v["name"] == "Mark McGwire");
    EXPECT(v["accomplishment"] == R"(Mark set a major league home run record in 1998.
)");
    EXPECT(v["stats"] == R"(65 Home Runs
0.278 Batting Average

)");
}


CASE("test_eckit_yaml_18") {
    Value v = YAMLParser::decodeString(R"(
plain:
  This unquoted scalar
  spans many lines.

quoted: "So does this
  quoted scalar.\n"

)");
    std::cout << to_json(v) << std::endl;
}


CASE("ECKIT-260") {
    // "true"/"false" is interpreted as a bool (rather than string) and comparators were broken.
    const auto _false = Value{false};
    const Value _true = Value{true};
    const Value _nil  = Value{};

    SECTION("Value{false}") {
        EXPECT(_false.typeName() == "Bool");
        EXPECT(_false == _false);
        for (const std::string& str : {"false", "False", "FALSE", "no", "off"}) {
            EXPECT(_false == YAMLParser::decodeString(str));
        }
    }

    SECTION("Value{true}") {
        EXPECT(_true.typeName() == "Bool");
        EXPECT(_true == _true);
        for (const std::string& str : {"true", "True", "TRUE", "yes", "on"}) {
            EXPECT(_true == YAMLParser::decodeString(str));
        }
    }

    SECTION("Value{}") {
        EXPECT(_nil.typeName() == "Nil");
        EXPECT(_nil == _nil);
        for (const std::string& str : {"null", "Null", "NULL", "~"}) {
            EXPECT(_nil == YAMLParser::decodeString(str));
        }
    }

    SECTION("Value.typeName() == \"String\"") {
        for (const std::string& str : {
                 "y",
                 "n",
                 "string",
                 "'12345'",
                 "\"42\"",
             }) {
            EXPECT(YAMLParser::decodeString(str).typeName() == "String");
        }
    }

    SECTION("Value.typeName() == \"Number\"") {
        for (const std::string& str : {
                 "12345",
                 "42",
             }) {
            EXPECT(YAMLParser::decodeString(str).typeName() == "Number");
        }
    }

    // Check ordering and comparisons
    EXPECT(_false != _true);
    EXPECT(_true != _false);

    EXPECT(_nil < _true);
    EXPECT(_nil < _false);
    EXPECT(_false < _true);
    EXPECT_NOT(_true < _true);
    EXPECT_NOT(_false < _false);
    EXPECT(Value("Something") == Value("Something"));
}


CASE("test_eckit_yaml_22") {
    Value v = YAMLParser::decodeString(R"(
canonical: 2001-12-15T02:59:43.1Z
iso8601: 2001-12-14t21:59:43.10-05:00
spaced: 2001-12-14 21:59:43.10 -5
date: 2002-12-14
)");
    std::cout << to_json(v) << std::endl;
}


#if 0
CASE("test_eckit_yaml_23") {
    Value v =  YAMLParser::decodeString(R"(
---
not-date: !!str 2002-04-28

picture: !!binary |
 R0lGODlhDAAMAIQAAP//9/X
 17unp5WZmZgAAAOfn515eXv
 Pz7Y6OjuDg4J+fn5OTk6enp
 56enmleECcgggoBADs=

application specific tag: !something |
 The semantics of the tag
 above may be different for
 different documents.
)");
std::cout << to_json(v) << std::endl;
}
#endif


#if 0
CASE("test_eckit_yaml_24") {
    Value v =  YAMLParser::decodeString(R"(
%TAG ! tag:clarkevans.com,2002:
--- !shape
  # Use the ! handle for presenting
  # tag:clarkevans.com,2002:circle
- !circle
  center: &ORIGIN {x: 73, y: 129}
  radius: 7
- !line
  start: *ORIGIN
  finish: { x: 89, y: 102 }
- !label
  start: *ORIGIN
  color: 0xFFEEBB
  text: Pretty vector drawing.
)");
    std::cout << to_json(v) << std::endl;
}
#endif


CASE("test_eckit_yaml_25") {
    Value v = YAMLParser::decodeString(R"(
# sets are represented as a
# mapping where each key is
# associated with the empty string
--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)");
    std::cout << to_json(v) << std::endl;
}


CASE("test_eckit_yaml_26") {
    Value v = YAMLParser::decodeString(R"(
# ordered maps are represented as
# a sequence of mappings, with
# each mapping having one key
--- !!omap
- Mark McGwire: 65
- Sammy Sosa: 63
- Ken Griffy: 58
)");
    std::cout << to_json(v) << std::endl;
}


CASE("test_eckit_yaml_27") {
    Value v = YAMLParser::decodeString(R"(
--- !<tag:clarkevans.com,2002:invoice>
invoice: 34843
date   : 2001-01-23
bill-to: &id001
    given  : Chris
    family : Dumars
    address:
        lines: |
            458 Walkman Dr.
            Suite #292
        city    : Royal Oak
        state   : MI
        postal  : 48046
ship-to: *id001
product:
    - sku         : BL394D
      quantity    : 4
      description : Basketball
      price       : 450.00
    - sku         : BL4438H
      quantity    : 1
      description : Super Hoop
      price       : 2392.00
tax  : 251.42
total: 4443.52
comments:
    Late afternoon is best.
    Backup contact is Nancy
    Billsmer @ 338-4338.
)");
    std::cout << to_json(v) << std::endl;
}


#if 0
CASE("test_eckit_yaml_28") {
    Value v =  YAMLParser::decodeString(R"(
---
Time: 2001-11-23 15:01:42 -5
User: ed
Warning:
  This is an error message
  for the log file
---
Time: 2001-11-23 15:02:31 -5
User: ed
Warning:
  A slightly different error
  message.
---
Date: 2001-11-23 15:03:17 -5
User: ed
Fatal:
  Unknown variable "bar"
Stack:
  - file: TopClass.py
    line: 23
    code: |
      x = MoreObject("345\n")
  - file: MoreClass.py
    line: 58
    code: |-
      foo = bar
)");
    std::cout << to_json(v) << std::endl;
}
#endif


CASE("numeric literals") {
    for (const std::string& str : {
             "1",
             "12",
         }) {
        EXPECT_EQUAL(YAMLParser::decodeString(str).typeName(), "Number");
        EXPECT_EQUAL(YAMLParser::decodeString("-" + str).typeName(), "Number");
    }

    for (const std::string& str : {
             "1.",     "12.",     ".1",     ".12",     "1.2",     "12.3",     "1.23",     "1e2",   "12e3",
             "1.e2",   "12.e3",   ".1e2",   ".12e3",   "1.2e3",   "12.3e4",   "1.23e4",   "1e-2",  "12e-3",
             "1.e-2",  "12.e-3",  ".1e-2",  ".12e-3",  "1.2e-3",  "12.3e-4",  "1.23e-4",  "1e23",  "12e34",
             "1.e23",  "12.e34",  ".1e23",  ".12e34",  "1.2e34",  "12.3e45",  "1.23e45",  "1e-23", "12e-34",
             "1.e-23", "12.e-34", ".1e-23", ".12e-34", "1.2e-34", "12.3e-45", "1.23e-45",
         }) {
        EXPECT_EQUAL(YAMLParser::decodeString(str).typeName(), "Double");
        EXPECT_EQUAL(YAMLParser::decodeString("-" + str).typeName(), "Double");
    }


    for (const std::string& str : {
             ".",  "e",  "e1",  "e12",  "e-1",  "e-12",  ".e1",  ".e12",  ".e-1",  ".e-12",
             "-.", "-e", "-e1", "-e12", "-e-1", "-e-12", "-.e1", "-.e12", "-.e-1", "-.e-12",
         }) {
        EXPECT_EQUAL(YAMLParser::decodeString(str).typeName(), "String");
    }
}


CASE("test_eckit_yaml_cfg_1") {
    Value v = YAMLParser::decodeFile("cfg.1.yaml");
    std::cout << to_json(v) << std::endl;
}


#if eckit_HAVE_UNICODE
CASE("test_eckit_yaml_unicode") {
    Value v = YAMLParser::decodeFile("unicode.yaml");
    std::cout << to_json(v) << std::endl;
    EXPECT(v["test1"] == v["test2"]);
}
#endif  // eckit_HAVE_UNICODE


CASE("test_eckit_yaml_comment_in_string") {
    Value v = YAMLParser::decodeFile("string.yaml");
    std::cout << to_json(v) << std::endl;
    EXPECT(v["test1"] == "#fff");
    EXPECT(v["test2"] == "#aaa");

    std::string e = "starts here and continues here and more";

    EXPECT(v["test3"] == e);
    EXPECT(v["test4"][0] == e);
    EXPECT(v["test5"]["test6"] == e);

    EXPECT(v["test6"] == "Quotes in 'strings' are special");

    EXPECT(bool(v["bool1"]) == true);
    EXPECT(bool(v["bool2"]) == false);
    EXPECT(bool(v["bool3"]) == true);
    EXPECT(bool(v["bool4"]) == false);
    EXPECT(bool(v["bool5"]) == true);
    EXPECT(bool(v["bool6"]) == false);
}


CASE("test_eckit_yaml_text") {
    using yj = std::pair<std::string, std::string>;

    for (
        const auto& yj : {
            yj{"foo : bar", R"({"foo":"bar"})"},
            {R"(
---
1: 2
165:
- 10u
- 10 metre u wind component
)",
             R"({1:2,165:["10u","10 metre u wind component"]})"},
            {R"(
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
)",
             R"({"base":{"name":"shared1","address":"shared2"},"foo":{"name":"shared1","address":"shared2","age":10},"bar":{"name":"shared1","address":"shared2","age":20}})"},
            {R"(
---
- '1'
- '2'
)",
             R"(["1","2"])"},
            {R"(---
foo:
    bar: 1
    spam: TRUE)",
             R"({"foo":{"bar":1,"spam":true}})"},
#if 0
            {R"(---
foo:
    bar: 1
  spam: TRUE)",
             R"({"foo":{"bar":1,"spam":true}})"},
#endif
        }) {
        Value v = YAMLParser::decodeString(yj.first);
        EXPECT(to_json(v) == yj.second);

        Value w = YAMLParser::decodeString(to_json(v));
        EXPECT(to_json(w) == yj.second);
    }
}

CASE("test_eckit_yaml_text_7") {
    auto q  = [](const std::string& s) { return "'" + s + "'"; };
    auto qq = [](const std::string& s) { return "\"" + s + "\""; };

    struct sq {
        std::string str;
        bool quote = true;
    };

    const auto* sep = ": ";
    const auto values
        = {sq{"foo", false}, sq{"foo "}, {"a b", false}, {" c"}, {"d "}, {"true"}, {"false"}, {"00"}, {"1"},
           /*{"e:f"}, */};

    for (const auto& k : values) {
        for (const auto& v : values) {
            const auto& key   = k.str;
            const auto& value = v.str;

            EXPECT(YAMLParser::decodeString(key + sep + value)[key] == value != (k.quote || v.quote));
            EXPECT(YAMLParser::decodeString(key + sep + q(value))[key] == value != k.quote);
            EXPECT(YAMLParser::decodeString(key + sep + qq(value))[key] == value != k.quote);

            EXPECT(YAMLParser::decodeString(q(key) + sep + value)[key] == value != v.quote);
            EXPECT(YAMLParser::decodeString(q(key) + sep + q(value))[key] == value);
            EXPECT(YAMLParser::decodeString(q(key) + sep + qq(value))[key] == value);

            EXPECT(YAMLParser::decodeString(qq(key) + sep + value)[key] == value != v.quote);
            EXPECT(YAMLParser::decodeString(qq(key) + sep + q(value))[key] == value);
            EXPECT(YAMLParser::decodeString(qq(key) + sep + qq(value))[key] == value);
        }
    }

    for (const std::string& yaml : {"a:\n b:\n  foo: bar", "a:\n b: {foo: bar}", "a: {b: {foo: bar}}"}) {
        EXPECT(YAMLParser::decodeString(yaml)["a"]["b"]["foo"] == "bar");
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
