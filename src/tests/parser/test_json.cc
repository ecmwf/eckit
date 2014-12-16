/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_parser

#include "ecbuild/boost_test_framework.h"

#include "eckit/parser/JSONParser.h"
#include "eckit/parser/JSON.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_parser_json )

BOOST_AUTO_TEST_CASE( test_eckit_parser_parse_to_value )
{
    std::istringstream in("{ \"a\" : [true, false, 3], \"b\" : 42.3 , \"c\" : null, \"d\" : \"y\n\tr\rh\", \"e\" : \"867017db84f4bc2b5078ca56ffd3b9b9\"}");
    JSONParser p(in);

    Value v = p.parse();

    std::cout << v         << std::endl;
    std::cout << v["a"]    << std::endl;
    std::cout << v["a"][2] << std::endl;

    JSON j(cout);
    j << v;

    ASSERT( v.isMap() );
    ASSERT( v.as<ValueMap>().size() == 5 );

    ASSERT( v["a"].isList() );
    ASSERT( v["a"].as<ValueList>().size() == 3 );


    ASSERT( v["a"][0].isBool() );
    ASSERT( v["a"][0].as<bool>() == true );

    ASSERT( v["a"][1].isBool() );
    ASSERT( v["a"][1].as<bool>() == false );

    ASSERT( v["a"][2].isNumber() );
    ASSERT( (int) v["a"][2] == 3 );

    ASSERT( v["b"].isDouble() );
    ASSERT( v["b"].as<double>() - 42.3 < 1E-12 );

    ASSERT( v["c"].isNil() );

    ASSERT( v["d"].isString() );

    ASSERT( v["e"].isString() );
    ASSERT( v["e"].as<string>() == "867017db84f4bc2b5078ca56ffd3b9b9" );

    std::cout << std::endl;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_parser_parse_to_map )
{
    istringstream in("{ \"a\" : \"AAA\", \"b\" : 0.0 , \"c\" : \"null\", \"d\" : \"\"}" );
    JSONParser p(in);
    Value v = p.parse();

    StringDict d;
    JSONParser::toStrDict(v,d);

    __print_container(std::cout,d);

    ASSERT( v.isMap() );
    ASSERT( v.as<ValueMap>().size() == 4 );

    ASSERT( v["a"].isString() );
    ASSERT( v["a"].as<string>() == "AAA" );

    ASSERT( v["b"].isDouble() );
    ASSERT( v["b"].as<double>() == 0.0 );

    ASSERT( v["c"].isString() );
    ASSERT( v["c"].as<string>() == "null" );

    ASSERT( v["d"].isString() );
    ASSERT( v["d"].as<string>() == "" );

    std::cout << std::endl;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
