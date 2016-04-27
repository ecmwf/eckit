/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_json_metadata

#include "ecbuild/boost_test_framework.h"

#include "eckit/parser/JSONMetadata.h"
#include "eckit/io/Buffer.h"

using namespace std;
using namespace eckit;


BOOST_AUTO_TEST_SUITE( test_eckit_json_metadata )

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_params )
{
    std::string json_str = "{\"key1\": 1, \"key2\": 99.5, \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::vector<std::string> params(md.keywords());

    BOOST_CHECK_EQUAL(params.size(), 3);
    BOOST_CHECK(std::find(params.begin(), params.end(), "key1") != params.end());
    BOOST_CHECK(std::find(params.begin(), params.end(), "key2") != params.end());
    BOOST_CHECK(std::find(params.begin(), params.end(), "key3") != params.end());
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_params_nondict )
{
    // If a valid json is supplied that is not an object/dict, there should be no
    // internal parameters to find

    std::string json_str = "[1, 2, 44.5]";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::vector<std::string> params(md.keywords());

    BOOST_CHECK_EQUAL(params.size(), 0);
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_has )
{
    std::string json_str = "{\"key1\": 1, \"key2\": 99.5, \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    BOOST_CHECK(md.has("key1"));
    BOOST_CHECK(md.has("key2"));
    BOOST_CHECK(md.has("key3"));
    BOOST_CHECK(!md.has("keyunused"));
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_has_nondict )
{
    // If a valid json is supplied that is not an object/dict, there should be no
    // internal parameters to find

    std::string json_str = "[1, 2, 44.5]";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    BOOST_CHECK(!md.has("key1"));
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_get_string )
{
    std::string json_str = "{\"key1\": 1, \"key2\": \"testing string\", \"key3\": []}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    std::string str_tmp;
    md.get("key2", str_tmp);

    // Gets value correctly
    BOOST_CHECK_EQUAL(str_tmp, "testing string");

    // Throws on incorrect type
    BOOST_CHECK_THROW(md.get("key3", str_tmp), BadCast);

    // Throws on missing key
    BOOST_CHECK_THROW(md.get("keyunused", str_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    BOOST_CHECK_THROW(md2.get("key1", str_tmp), AssertionFailed);
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_get_double )
{
    std::string json_str = "{\"key1\": 123.45, \"key2\": \"testing string\", \"key3\": [], \"key4\": 123}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    double dbl_tmp;
    md.get("key1", dbl_tmp);
    BOOST_CHECK_CLOSE(dbl_tmp, 123.45, 1.0e-6);

    // N.b. this should also work for integers.
    md.get("key4", dbl_tmp);
    BOOST_CHECK_CLOSE(dbl_tmp, 123, 1.0e-6);

    // Throws on incorrect type
    BOOST_CHECK_THROW(md.get("key3", dbl_tmp), BadCast);

    // Throws on missing key
    BOOST_CHECK_THROW(md.get("keyunused", dbl_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    BOOST_CHECK_THROW(md2.get("key1", dbl_tmp), AssertionFailed);
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_json_metadata_get_long )
{
    std::string json_str = "{\"key1\": 123.45, \"key2\": \"testing string\", \"key3\": [], \"key4\": 123}";
    Buffer buf(json_str.c_str(), json_str.length());

    JSONMetadata md(buf);

    long long_tmp;
    md.get("key4", long_tmp);
    BOOST_CHECK_EQUAL(long_tmp, 123);

    // Throws on incorrect type
    BOOST_CHECK_THROW(md.get("key3", long_tmp), BadCast);

    // Throws on missing key
    BOOST_CHECK_THROW(md.get("keyunused", long_tmp), OutOfRange);

    // Throws on non-dictionary root-value
    std::string json_str2 = "[1234]";
    Buffer buf2(json_str2.c_str(), json_str2.length());
    JSONMetadata md2(buf2);

    BOOST_CHECK_THROW(md2.get("key1", long_tmp), AssertionFailed);
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
