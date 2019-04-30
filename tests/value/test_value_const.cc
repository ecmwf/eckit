/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/testing/Test.h"

#include "eckit/parser/JSONParser.h"
#include "eckit/value/Value.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

Value foo() {
    const Value y = JSONParser::decodeString("{\"foo\":true}");
    Value x       = y;
    x             = x["foo"];
    return x;
}

CASE("ECKIT-260 : Assign a value to another accessed on a map") {

    Value x = foo();

    EXPECT(x.as<bool>() == true);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("ECKIT-260 : Assign a value to another accessed on a map") {

    Value x = JSONParser::decodeString("{\"foo\":true}");
    x       = x["foo"];

    EXPECT(x.as<bool>() == true);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("ECKIT-265 : Date comparison in values is correctly ordered") {

    EXPECT(Value(Date(2016, 3, 31)) == Value(Date(2016, 3, 31)));
    EXPECT(Value(Date(2017, 3, 31)) > Value(Date(2016, 3, 31)));
    EXPECT(Value(Date(2016, 3, 31)) < Value(Date(2017, 3, 31)));

    EXPECT(Value(Date(2016, 3, 31)) >= Value(Date(2016, 3, 31)));
    EXPECT(Value(Date(2016, 3, 31)) <= Value(Date(2016, 3, 31)));

    EXPECT(!(Value(Date(2016, 3, 31)) > Value(Date(2017, 3, 31))));
    EXPECT(!(Value(Date(2017, 3, 31)) < Value(Date(2017, 3, 31))));

    EXPECT(Value(Date(2016, 5, 1)).compare(Value(Date(2016, 5, 1))) == 0);
    EXPECT(Value(Date(2016, 3, 1)).compare(Value(Date(2016, 5, 1))) == -1);
    EXPECT(Value(Date(2016, 11, 1)).compare(Value(Date(2016, 5, 1))) == 1);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
