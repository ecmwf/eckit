/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/testing/Test.h"

#include "eckit/value/Value.h"
#include "eckit/parser/JSONParser.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

Value foo() {
  const Value y = JSONParser::decodeString( "{\"foo\":true}" );
  Value x = y;
  x =  x["foo"];
  return x;
}

CASE( "ECKIT-260 : Assign a value to another accessed on a map" ) {

    Value x = foo();

    EXPECT( x.as<bool>() == true );
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "ECKIT-260 : Assign a value to another accessed on a map" ) {

    Value x = JSONParser::decodeString( "{\"foo\":true}" );
    x = x["foo"];

    EXPECT( x.as<bool>() == true );
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "ECKIT-265 : Date comparison in values is correctly ordered" ) {

    EXPECT( Value(Date(2016, 3, 31)) == Value(Date(2016, 3, 31)) );
    EXPECT( Value(Date(2017, 3, 31)) > Value(Date(2016, 3, 31)) );
    EXPECT( Value(Date(2016, 3, 31)) < Value(Date(2017, 3, 31)) );

    EXPECT( Value(Date(2016, 3, 31)) >= Value(Date(2016, 3, 31)) );
    EXPECT( Value(Date(2016, 3, 31)) <= Value(Date(2016, 3, 31)) );

    EXPECT( !(Value(Date(2016, 3, 31)) > Value(Date(2017, 3, 31))) );
    EXPECT( !(Value(Date(2017, 3, 31)) < Value(Date(2017, 3, 31))) );

    EXPECT( Value(Date(2016, 5, 1)).compare(Value(Date(2016, 5, 1))) == 0);
    EXPECT( Value(Date(2016, 3, 1)).compare(Value(Date(2016, 5, 1))) == -1);
    EXPECT( Value(Date(2016,11, 1)).compare(Value(Date(2016, 5, 1))) == 1);
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "There is a Total Order between types" ) {

    // Total order between types:
    // bool [ true > false ] > number > string > nil > list > map > orderedmap > Date > Time > DateTime

    EXPECT( Value(true) > Value(false) );
    EXPECT( Value(false) > Value(int(1)) );
    EXPECT( Value(int(2)) > Value(std::string("foo")) );
    EXPECT( Value(std::string("foo")) > Value() );
    EXPECT( Value() > Value::makeList() );
    EXPECT( Value::makeList() > Value::makeMap() );
    EXPECT( Value::makeMap() > Value(Date(2016,5,1)) );
    EXPECT( Value(Date(2016,5,1)) > Value(Time(1000)) );
    EXPECT( Value(Time(1000)) > Value(DateTime()) );
    EXPECT( Value(DateTime()) > Value::makeOrderedMap() );
}

//----------------------------------------------------------------------------------------------------------------------

CASE( "Ordered Maps compare using their ordered keys" ) {

    EXPECT( Value(2) > Value(1) );

    Value small = Value::makeOrderedMap();

    small["1"] = 2;
    small["2"] = 3;
    small["3"] = 4;

    Value large = Value::makeOrderedMap();

    large["2"] = 2;
    large["3"] = 3;
    large["4"] = 4;

    EXPECT( large.compare(large) ==  0 );
    EXPECT( large.compare(small) ==  1 );
    EXPECT( small.compare(large) == -1 );
    EXPECT( small.compare(small) ==  0 );

    EXPECT( large > small );
    EXPECT( small < large );
    EXPECT( small != large );
    EXPECT( large != small );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
