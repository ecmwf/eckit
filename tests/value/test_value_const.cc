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

CASE( "ECKIT-260 : Assign a value to another accessed on a map" ) {

    Value x = JSONParser::decodeString( "{\"foo\":true}" );
    x = x["foo"];

    EXPECT( x.as<bool>() == true );
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
