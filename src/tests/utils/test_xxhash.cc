/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_utils

#include <iostream>

#include "ecbuild/boost_test_framework.h"

#include "eckit/utils/xxHash.h"
#include "eckit/log/Timer.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"

using namespace std;
using namespace eckit;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_utils_xxHash )

BOOST_AUTO_TEST_CASE( test_eckit_utils_xxHash_constructor )
{
    xxHash hash;

        const char* msg = "The quick brown fox jumps over the lazy dog";

        hash.add(msg,strlen(msg));

        std::string res ("0b242d361fda71bc");

        BOOST_CHECK_EQUAL( res , hash.digest() );
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_xxHash_constructor_string )
{
    xxHash hash( "Nobody inspects the spammish repetition" );

    std::string res ("fbcea83c8a378bf1");

    BOOST_CHECK_EQUAL( res , hash.digest() );
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_xxHash_test_suite )
{
    BOOST_CHECK_EQUAL( xxHash("").digest(), std::string("ef46db3751d8e999"));
    BOOST_CHECK_EQUAL( xxHash("a").digest(), std::string("d24ec4f1a98c6e5b"));
    BOOST_CHECK_EQUAL( xxHash("abc").digest(), std::string("44bc2cf5ad770999"));
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_xxhash_compute )
{
    std::string msg ( "The quick brown fox jumps over the lazy dog" );

    std::string res ("0b242d361fda71bc");

    xxHash hash;
    BOOST_CHECK_EQUAL( res , hash.compute(msg.c_str(), msg.size()));
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_xxhash_reset )
{
    xxHash hash( "FOOBAR" );

    hash.reset(); // reset initial state

    std::string msg ( "The quick brown fox jumps over the lazy dog" );

    hash.add(msg.c_str(), msg.size());

    std::string res ("0b242d361fda71bc");

    BOOST_CHECK_EQUAL( res , hash.digest());
}

BOOST_AUTO_TEST_SUITE_END()


//----------------------------------------------------------------------------------------------------------------------


} // end namespace test
} // end namespace eckit
