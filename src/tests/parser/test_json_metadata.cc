/*
 * (C) Copyright 1996-2015 ECMWF.
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

    std::vector<std::string> params(md.parameters());

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

    std::vector<std::string> params(md.parameters());

    BOOST_CHECK_EQUAL(params.size(), 0);
}

// --------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
