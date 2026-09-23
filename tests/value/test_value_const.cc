// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/parser/JSONParser.h"
#include "eckit/testing/Test.h"
#include "eckit/value/Value.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

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

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
