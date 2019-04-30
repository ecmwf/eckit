/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <limits>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/Length.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/types/Date.h"
#include "eckit/value/Properties.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit_test {

//----------------------------------------------------------------------------------------------------------------------


CASE("Test serialization") {
    eckit::Log::info() << "(de)serialize Properties to/from file" << std::endl;
    PathName filename    = PathName::unique("data");
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
    ValueMap m;
    m.insert(std::make_pair("int", numeric_limits<int>::max()));
    m.insert(std::make_pair("unsigned int", numeric_limits<unsigned int>::max()));
    m.insert(std::make_pair("long long", numeric_limits<long long>::max()));
    m.insert(std::make_pair("unsigned long long", numeric_limits<unsigned long long>::max()));
    m.insert(std::make_pair("double", numeric_limits<double>::max()));
    m.insert(std::make_pair("string", "foo"));
    m.insert(std::make_pair("Length", Length(42)));
    m.insert(std::make_pair("PathName", PathName("/var/tmp")));
    m.insert(std::make_pair("Vector", ValueList(5, "string")));
    p.set("Map", Value(m));

    Properties pm;
    pm.set("int", numeric_limits<int>::max());
    pm.set("unsigned int", numeric_limits<unsigned int>::max());
    p.set("Nested", pm);

    std::vector<Properties> property_list(2);
    property_list[0].set("int", numeric_limits<int>::max());
    property_list[1].set("string", "foo");
    p.set("list", toValue(property_list));

    eckit::Log::info() << "encoded Properties: " << p << std::endl;
    {
        FileStream sout(filepath.c_str(), "w");
        auto c = closer(sout);
        sout << p;
    }
    {
        FileStream sin(filepath.c_str(), "r");
        auto c = closer(sin);
        Properties p2(sin);
        eckit::Log::info() << "decoded Properties: " << p2 << std::endl;
        EXPECT((bool)p["bool"] == (bool)p2["bool"]);
        EXPECT(p["int"] == p2["int"]);
        EXPECT(p["unsigned int"] == p2["unsigned int"]);
        EXPECT(p["long long"] == p2["long long"]);
        EXPECT(p["unsigned long long"] == p2["unsigned long long"]);
        EXPECT(p["double"] == p2["double"]);
        EXPECT(p["string"] == p2["string"]);
        EXPECT(p["Length"] == p2["Length"]);
        EXPECT(p["Date"] == p2["Date"]);
        // EXPECT(p["Time"] == p2["Time"]);  <-- not implemented
        // EXPECT(p["DateTime"] == p2["DateTime"]); <-- not implemented
        EXPECT(p["PathName"] == p2["PathName"]);
        EXPECT(p["Vector"] == p2["Vector"]);
        EXPECT(p["Map"] == p2["Map"]);
    }
    if (filename.exists())
        filename.unlink();

    Properties access_nested = p.get("Nested");

    eckit::ValueList access_list = p.get("list");
    eckit::Log::info() << "encoded list: " << access_list << std::endl;
    std::vector<eckit::Properties> access_property_list(access_list.begin(), access_list.end());

    eckit::Log::info() << "encoded Nested: " << access_nested << std::endl;
    {
        FileStream sout(filepath.c_str(), "w");
        auto c = closer(sout);
        sout << p;
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
