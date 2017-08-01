/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_parser

#include "ecbuild/boost_test_framework.h"

//#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/parser/JSON.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;

using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_parser_yaml )

//----------------------------------------------------------------------------------------------------------------------

static std::string toJSON(const Value& v) {
    std::ostringstream oss;
    JSON json(oss);
    json << v;
    return oss.str();
}

// BOOST_AUTO_TEST_CASE( test_eckit_yaml_1 ) {
//     Value v =  YAMLParser::decodeFile("2.1.yaml");
//     std::cout << "2.1.yaml " << v << std::endl;
//     std::cout << toJSON(v) << std::endl;

//     BOOST_CHECK( v.isList() );
//     BOOST_CHECK_EQUAL( v.size(), 3 );

//     BOOST_CHECK_EQUAL( v[0], "Mark McGwire" );
//     BOOST_CHECK_EQUAL( v[1], "Sammy Sosa" );
//     BOOST_CHECK_EQUAL( v[2], "Ken Griffey" );

//     // BOOST_CHECK_EQUAL(toJSON(v), "[\"Mark McGwire\",\"Sammy Sosa\",\"Ken Griffey\"]");

// }

BOOST_AUTO_TEST_CASE( test_eckit_yaml_2 ) {
    Value v =  YAMLParser::decodeFile("2.2.yaml");
    std::cout << "2.2.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    BOOST_CHECK( v.isMap() );
    BOOST_CHECK_EQUAL( v.keys().size(), 3 );

    BOOST_CHECK_EQUAL( v["hr"], Value(65) );
    BOOST_CHECK_EQUAL( v["avg"], Value(0.278) );
    BOOST_CHECK_EQUAL( v["rbi"], Value(147) );
}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_3 ) {
    Value v =  YAMLParser::decodeFile("2.3.yaml");
    std::cout << "2.3.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    BOOST_CHECK( v.isMap() );
    BOOST_CHECK_EQUAL( v.keys().size(), 2 );

    BOOST_CHECK( v["american"].isList() );
    BOOST_CHECK_EQUAL( v["american"].size(), 3 );

    BOOST_CHECK( v["national"].isList() );
    BOOST_CHECK_EQUAL( v["national"].size(), 3 );
}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_4 ) {
    Value v =  YAMLParser::decodeFile("2.4.yaml");
    std::cout << "2.4.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    BOOST_CHECK( v.isList() );
    BOOST_CHECK_EQUAL( v.size(), 2 );

    BOOST_CHECK( v[0].isMap() );
    BOOST_CHECK_EQUAL( v[0].keys().size(), 3 );

    BOOST_CHECK( v[1].isMap() );
    BOOST_CHECK_EQUAL( v[1].keys().size(), 3 );

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_5 ) {
    Value v =  YAMLParser::decodeFile("2.5.yaml");
    std::cout << "2.5.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    BOOST_CHECK( v.isList() );
    BOOST_CHECK_EQUAL( v.size(), 3 );

    BOOST_CHECK( v[0].isList() );
    BOOST_CHECK_EQUAL( v[0].size(), 3 );

    BOOST_CHECK( v[1].isList() );
    BOOST_CHECK_EQUAL( v[1].size(), 3 );


    BOOST_CHECK( v[2].isList() );
    BOOST_CHECK_EQUAL( v[2].size(), 3 );

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_6 ) {
    Value v =  YAMLParser::decodeFile("2.6.yaml");
    std::cout << "2.6.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    BOOST_CHECK( v.isMap() );
    BOOST_CHECK_EQUAL( v.keys().size(), 2 );

    BOOST_CHECK( v["Mark McGwire"].isMap() );
    BOOST_CHECK_EQUAL( v["Mark McGwire"].keys().size(), 2 );

    BOOST_CHECK( v["Sammy Sosa"].isMap() );
    BOOST_CHECK_EQUAL( v["Sammy Sosa"].keys().size(), 2 );


}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_7 ) {
    Value v =  YAMLParser::decodeFile("2.7.yaml");
    std::cout << "2.7.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_8 ) {
    Value v =  YAMLParser::decodeFile("2.8.yaml");
    std::cout << "2.8.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_9 ) {
    Value v =  YAMLParser::decodeFile("2.9.yaml");
    std::cout << "2.9.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_10 ) {
    Value v =  YAMLParser::decodeFile("2.10.yaml");
    std::cout << "2.10.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

}

/*
BOOST_AUTO_TEST_CASE( test_eckit_yaml_11 ) {
    Value v =  YAMLParser::decodeFile("2.11.yaml");
    std::cout << "2.11.yaml " << v << std::endl;
        std::cout << toJSON(v) << std::endl;
            std::cout << toJSON(v) << std::endl;

}
*/

BOOST_AUTO_TEST_CASE( test_eckit_yaml_12 ) {
    Value v =  YAMLParser::decodeFile("2.12.yaml");
    std::cout << "2.12.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}
/*
BOOST_AUTO_TEST_CASE( test_eckit_yaml_13 ) {
    Value v =  YAMLParser::decodeFile("2.13.yaml");
    std::cout << "2.13.yaml " << v << std::endl;
        std::cout << toJSON(v) << std::endl;
}
*/

BOOST_AUTO_TEST_CASE( test_eckit_yaml_14 ) {
    Value v =  YAMLParser::decodeFile("2.14.yaml");
    std::cout << "2.14.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    std::cout << "----------" << std::endl;
    std::cout << v << std::endl;
    std::cout << "----------" << std::endl;

    BOOST_CHECK_EQUAL(v, "Mark McGwire's year was crippled by a knee injury.");
}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_15 ) {
    Value v =  YAMLParser::decodeFile("2.15.yaml");
    std::cout << "2.15.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;

    std::cout << "----------" << std::endl;
    std::cout << v << std::endl;
    std::cout << "----------" << std::endl;

    BOOST_CHECK_EQUAL(v,
                      "Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting Average\n\nWhat a year!\n");

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_16 ) {
    Value v =  YAMLParser::decodeFile("2.16.yaml");
    std::cout << "2.16.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

// BOOST_AUTO_TEST_CASE( test_eckit_yaml_17 ) {
//     Value v =  YAMLParser::decodeFile("2.17.yaml");
//     std::cout << "2.17.yaml " << v << std::endl;
//         std::cout << toJSON(v) << std::endl;
// }

BOOST_AUTO_TEST_CASE( test_eckit_yaml_18 ) {
    Value v =  YAMLParser::decodeFile("2.18.yaml");
    std::cout << "2.18.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_19 ) {
    Value v =  YAMLParser::decodeFile("2.19.yaml");
    std::cout << "2.19.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;


    BOOST_CHECK_EQUAL(v["canonical"], Value(12345));
    // BOOST_CHECK_EQUAL(v["decimal"], Value(12345));
    // BOOST_CHECK_EQUAL(v["sexagesimal"], Value(12345));
    BOOST_CHECK_EQUAL(v["octal"], Value(12));
    BOOST_CHECK_EQUAL(v["hexadecimal"], Value(12));


// canonical: 12345
// decimal: +12,345
// sexagesimal: 3:25:45
// octal: 014
// hexadecimal: 0xC

}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_20 ) {
    Value v =  YAMLParser::decodeFile("2.20.yaml");
    std::cout << "2.20.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


BOOST_AUTO_TEST_CASE( test_eckit_yaml_21 ) {
    Value v =  YAMLParser::decodeFile("2.21.yaml");
    std::cout << "2.21.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}

BOOST_AUTO_TEST_CASE( test_eckit_yaml_22 ) {
    Value v =  YAMLParser::decodeFile("2.22.yaml");
    std::cout << "2.22.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


// BOOST_AUTO_TEST_CASE( test_eckit_yaml_23 ) {
//     Value v =  YAMLParser::decodeFile("2.23.yaml");
//     std::cout << "2.23.yaml " << v << std::endl;
// std::cout << toJSON(v) << std::endl;
// }



// BOOST_AUTO_TEST_CASE( test_eckit_yaml_24 ) {
//     Value v =  YAMLParser::decodeFile("2.24.yaml");
//     std::cout << "2.24.yaml " << v << std::endl;
// std::cout << toJSON(v) << std::endl;
// }


BOOST_AUTO_TEST_CASE( test_eckit_yaml_25 ) {
    Value v =  YAMLParser::decodeFile("2.25.yaml");
    std::cout << "2.25.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


BOOST_AUTO_TEST_CASE( test_eckit_yaml_26 ) {
    Value v =  YAMLParser::decodeFile("2.26.yaml");
    std::cout << "2.26.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


BOOST_AUTO_TEST_CASE( test_eckit_yaml_27 ) {
    Value v =  YAMLParser::decodeFile("2.27.yaml");
    std::cout << "2.27.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


// BOOST_AUTO_TEST_CASE( test_eckit_yaml_28 ) {
//     Value v =  YAMLParser::decodeFile("2.28.yaml");
//     std::cout << "2.28.yaml " << v << std::endl;
//     std::cout << toJSON(v) << std::endl;
// }

BOOST_AUTO_TEST_CASE( test_eckit_yaml_cfg_1 ) {
    Value v =  YAMLParser::decodeFile("cfg.1.yaml");
    std::cout << "cfg.1.yaml " << v << std::endl;
    std::cout << toJSON(v) << std::endl;
}


BOOST_AUTO_TEST_CASE( test_eckit_yaml_text_1 ) {

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

    Value v =  YAMLParser::decodeString(text);
     v.dump(std::cout) << std::endl;
}


BOOST_AUTO_TEST_CASE( test_eckit_yaml_text_2 ) {

    const char* text = R"YAML(
---
1: 2
)YAML";

    Value v =  YAMLParser::decodeString(text);
     v.dump(std::cout) << std::endl;

}
//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
