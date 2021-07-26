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
#include "eckit/config/LibEcKit.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

void write_json(eckit::JSON& json) {
    json.startObject();
    json << "a" << 1;
    json << "b";
    {
        json.startObject();
        json << "b1" << 2;
        json << "b2" << 3;
        json.endObject();
    }
    json << "c" << 4;
    json << "d";
    {
        json.startList();
        json << 1 << 2 << 3 << 4 << 5;
        json.endList();
    }
    json.endObject();
}

CASE("compact (default)") {
    std::stringstream s;
    JSON json(s);
    write_json(json);
    EXPECT(s.str() == "{\"a\":1,\"b\":{\"b1\":2,\"b2\":3},\"c\":4,\"d\":[1,2,3,4,5]}");
}

CASE("compact (explicit)") {
    JSON::Formatting formatting(JSON::Formatting::COMPACT);

    std::stringstream s;
    JSON json(s, formatting);
    write_json(json);
    EXPECT(s.str() == "{\"a\":1,\"b\":{\"b1\":2,\"b2\":3},\"c\":4,\"d\":[1,2,3,4,5]}");
}

CASE("indent(2) --> INDENT_DICT, indent=2") {
    std::stringstream s;
    JSON json(s, JSON::Formatting::indent(2));
    write_json(json);
    EXPECT(s.str() ==
           "{\n"
           "  \"a\" : 1,\n"
           "  \"b\" : {\n"
           "    \"b1\" : 2,\n"
           "    \"b2\" : 3\n"
           "  },\n"
           "  \"c\" : 4,\n"
           "  \"d\" : [1,2,3,4,5]\n"
           "}");
}

CASE("indent(4) --> INDENT_DICT, indent=4") {
    std::stringstream s;
    JSON json(s, JSON::Formatting::indent(4));
    write_json(json);
    EXPECT(s.str() ==
           "{\n"
           "    \"a\" : 1,\n"
           "    \"b\" : {\n"
           "        \"b1\" : 2,\n"
           "        \"b2\" : 3\n"
           "    },\n"
           "    \"c\" : 4,\n"
           "    \"d\" : [1,2,3,4,5]\n"
           "}");
}

CASE("INDENT_LIST, indent=2") {
    JSON::Formatting formatting(JSON::Formatting::INDENT_LIST);

    std::stringstream s;
    JSON json(s, formatting);
    write_json(json);
    EXPECT(s.str() ==
           "{\"a\" : 1,\"b\" : {\"b1\" : 2,\"b2\" : 3},\"c\" : 4,\"d\" : [\n"
           "  1,\n"
           "  2,\n"
           "  3,\n"
           "  4,\n"
           "  5\n"
           "]}");
}

CASE("INDENT_DICT, indent=2") {
    JSON::Formatting formatting(JSON::Formatting::INDENT_DICT);

    std::stringstream s;
    JSON json(s, formatting);
    write_json(json);
    EXPECT(s.str() ==
           "{\n"
           "  \"a\" : 1,\n"
           "  \"b\" : {\n"
           "    \"b1\" : 2,\n"
           "    \"b2\" : 3\n"
           "  },\n"
           "  \"c\" : 4,\n"
           "  \"d\" : [1,2,3,4,5]\n"
           "}");
}


CASE("INDENT_ALL, indent=2") {
    JSON::Formatting formatting(JSON::Formatting::INDENT_ALL);

    std::stringstream s;
    JSON json(s, formatting);
    write_json(json);
    EXPECT(s.str() ==
           "{\n"
           "  \"a\" : 1,\n"
           "  \"b\" : {\n"
           "    \"b1\" : 2,\n"
           "    \"b2\" : 3\n"
           "  },\n"
           "  \"c\" : 4,\n"
           "  \"d\" : [\n"
           "    1,\n"
           "    2,\n"
           "    3,\n"
           "    4,\n"
           "    5\n"
           "  ]\n"
           "}");
}

CASE("INDENT_DICT | INDENT_LIST, indent=4") {
    JSON::Formatting formatting(JSON::Formatting::INDENT_DICT | JSON::Formatting::INDENT_LIST, 4);

    std::stringstream s;
    JSON json(s, formatting);
    write_json(json);
    EXPECT(s.str() ==
           "{\n"
           "    \"a\" : 1,\n"
           "    \"b\" : {\n"
           "        \"b1\" : 2,\n"
           "        \"b2\" : 3\n"
           "    },\n"
           "    \"c\" : 4,\n"
           "    \"d\" : [\n"
           "        1,\n"
           "        2,\n"
           "        3,\n"
           "        4,\n"
           "        5\n"
           "    ]\n"
           "}");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
