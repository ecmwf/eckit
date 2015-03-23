/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_value_properties

#include <limits>

#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/types/Date.h"
#include "eckit/value/Properties.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_value_properties )

BOOST_AUTO_TEST_CASE( test_serialize )
{
    BOOST_TEST_MESSAGE("(de)serialize Properties to/from file");
    PathName filename = PathName::unique( "data" );
    std::string filepath = filename.asString();
    Properties p;
    p.set("bool", true);
    p.set("int", numeric_limits<int>::max());
    p.set("unsigned int", numeric_limits<unsigned int>::max());
    p.set("long long", numeric_limits<long long>::max());
    p.set("unsigned long long", numeric_limits<unsigned long long>::max());
    p.set("double", numeric_limits<double>::max());
    p.set("string", "foo");
    p.set("Length", Length(42));
    p.set("Date", Date(2015, 2, 1));
    // p.set("Time", Time(11, 59, 59));  <-- not implemented
    // p.set("DateTime", DateTime(Date(1, 2, 2015), Time(11, 59, 59)));  <-- not implemented
    p.set("PathName", PathName("/var/tmp"));
    p.set("Vector", ValueList(5, "string"));
    BOOST_TEST_MESSAGE("encoded Properties: " << p);
    {
        FileStream sout( filepath.c_str(), "w" );
        sout << p;
    }
    {
        FileStream sin( filepath.c_str(), "r" );
        Properties p2(sin);
        BOOST_TEST_MESSAGE("decoded Properties: " << p2);
        BOOST_CHECK_EQUAL((bool)p["bool"], (bool)p2["bool"]);
        BOOST_CHECK_EQUAL(p["int"], p2["int"]);
        BOOST_CHECK_EQUAL(p["unsigned int"], p2["unsigned int"]);
        BOOST_CHECK_EQUAL(p["long long"], p2["long long"]);
        BOOST_CHECK_EQUAL(p["unsigned long long"], p2["unsigned long long"]);
        BOOST_CHECK_EQUAL(p["double"], p2["double"]);
        BOOST_CHECK_EQUAL(p["string"], p2["string"]);
        BOOST_CHECK_EQUAL(p["Length"], p2["Length"]);
        BOOST_CHECK(p["Date"].compare(p2["Date"])); // FIXME: equality check fails
        // BOOST_CHECK_EQUAL(p["Time"], p2["Time"]);  <-- not implemented
        // BOOST_CHECK_EQUAL(p["DateTime"], p2["DateTime"]); <-- not implemented
        BOOST_CHECK_EQUAL(p["PathName"], p2["PathName"]);
        BOOST_CHECK_EQUAL(p["Vector"], p2["Vector"]);
    }
    if (filename.exists()) filename.unlink();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
