/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>

#include "eckit/io/Buffer.h"
#include "eckit/parser/JSONMetadata.h"
#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::types;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_json_metadata_params") {
    std::string json_str = "{\"key1\": 1, \"key2\": 99.5, \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::vector<std::string> params(md.keywords());

    EXPECT(params.size() == 3);
    EXPECT(std::find(params.begin(), params.end(), "key1") != params.end());
    EXPECT(std::find(params.begin(), params.end(), "key2") != params.end());
    EXPECT(std::find(params.begin(), params.end(), "key3") != params.end());
}

CASE("test_eckit_json_metadata_params_nondict") {
    // If a valid json is supplied that is not an object/dict, there should be no
    // internal parameters to find

    std::string json_str = "[1, 2, 44.5]";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::vector<std::string> params(md.keywords());

    EXPECT(params.size() == 0);
}

CASE("test_eckit_json_metadata_has") {
    std::string json_str = "{\"key1\": 1, \"key2\": 99.5, \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    EXPECT(md.has("key1"));
    EXPECT(md.has("key2"));
    EXPECT(md.has("key3"));
    EXPECT(!md.has("keyunused"));
}

CASE("test_eckit_json_metadata_has_nondict") {
    // If a valid json is supplied that is not an object/dict, there should be no
    // internal parameters to find

    std::string json_str = "[1, 2, 44.5]";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    EXPECT(!md.has("key1"));
}

CASE("test_eckit_json_metadata_get_string") {
    std::string json_str = "{\"key1\": 1, \"key2\": \"testing string\", \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::string str_tmp;
    md.get("key2", str_tmp);

    // Gets value correctly
    EXPECT(str_tmp == "testing string");

    // Throws on incorrect type
    EXPECT_THROWS_AS(md.get("key3", str_tmp), BadCast);

    // Throws on missing key
    EXPECT_THROWS_AS(md.get("keyunused", str_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    EXPECT_THROWS_AS(md2.get("key1", str_tmp), AssertionFailed);
}

CASE("test_eckit_json_metadata_get_double") {
    std::string json_str = "{\"key1\": 123.45, \"key2\": \"testing string\", \"key3\": [], \"key4\": 123}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    double dbl_tmp;
    md.get("key1", dbl_tmp);
    EXPECT(is_approximately_equal(dbl_tmp, 123.45, 1.0e-6));

    md.get("key4", dbl_tmp);
    EXPECT(is_approximately_equal(dbl_tmp, static_cast<double>(123), 1.0e-6));

    // Throws on incorrect type
    EXPECT_THROWS_AS(md.get("key3", dbl_tmp), BadCast);

    // Throws on missing key
    EXPECT_THROWS_AS(md.get("keyunused", dbl_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    EXPECT_THROWS_AS(md2.get("key1", dbl_tmp), AssertionFailed);
}

CASE("test_eckit_json_metadata_get_long") {
    std::string json_str = "{\"key1\": 123.45, \"key2\": \"testing string\", \"key3\": [], \"key4\": 123}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    long long_tmp;
    md.get("key4", long_tmp);
    EXPECT(long_tmp == 123);

    // Throws on incorrect type
    EXPECT_THROWS_AS(md.get("key3", long_tmp), BadCast);

    // Throws on missing key
    EXPECT_THROWS_AS(md.get("keyunused", long_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    EXPECT_THROWS_AS(md2.get("key1", long_tmp), AssertionFailed);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
