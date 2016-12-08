/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_utils

#include "ecbuild/boost_test_framework.h"

#include "eckit/utils/MD5.h"
#include "eckit/log/Log.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {};

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_utils_md5 )

BOOST_AUTO_TEST_CASE( test_eckit_utils_md5_constructor )
{
	MD5 md5;

	const char* msg = "The quick brown fox jumps over the lazy dog";

	md5.add(msg,strlen(msg));

	std::string res ("9e107d9d372bb6826bd81d3542a419d6");

//	ECKIT_DEBUG_VAR( md5.digest() );

	BOOST_CHECK_EQUAL( res , md5.digest() );
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_md5_constructor_string )
{
	MD5 md5( "Few quips galvanized the mock jury box" );

	std::string res ("01190cddf60f758278c728e768d218ff");

	BOOST_CHECK_EQUAL( res , md5.digest() );
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_md5_double_add )
{
	MD5 md5;

	const char* msg = "The quick brown fox jumps over the lazy dog";

	md5.add(msg,strlen(msg));
	md5.add(msg,strlen(msg));

	std::string res ("d27c6d8bcaa695e377d32387e115763c");

	BOOST_CHECK_EQUAL( res , md5.digest() );
}

// original test suite from RFC-1321

BOOST_AUTO_TEST_CASE( test_eckit_utils_md5_test_suite_from_rfc1321 )
{
    BOOST_CHECK_EQUAL( MD5("").digest(),
                       "d41d8cd98f00b204e9800998ecf8427e" );

    BOOST_CHECK_EQUAL( MD5("a").digest(),
                       "0cc175b9c0f1b6a831c399e269772661" );

    BOOST_CHECK_EQUAL( MD5("abc").digest(),
                       "900150983cd24fb0d6963f7d28e17f72" );

    BOOST_CHECK_EQUAL( MD5("message digest").digest(),
                       "f96b697d7cb7938d525a2f31aaf161d0" );

    BOOST_CHECK_EQUAL( MD5("abcdefghijklmnopqrstuvwxyz").digest(),
                       "c3fcd3d76192e4007dfb496cca67e13b" );

    BOOST_CHECK_EQUAL( MD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789").digest(),
                       "d174ab98d277d9f5a5611c2c9f419d9f" );

    BOOST_CHECK_EQUAL( MD5("12345678901234567890123456789012345678901234567890123456789012345678901234567890").digest(),
                       "57edf4a22be3c955ac49da2e2107b67a" );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
