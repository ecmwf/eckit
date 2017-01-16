/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_types

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/types/UUID.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_types_uuid )

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_uuid_default_constructor ) {
    UUID uuid;

    BOOST_CHECK_EQUAL( uuid.size() , 16 );

    BOOST_CHECK( uuid.isNil() );

    std::string res ("00000000000000000000000000000000");

    BOOST_CHECK_EQUAL( res , uuid.asString() );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_uuid_constructor_string ) {
    std::string s ("4b4053dc93e0b52a6f028cb36649d229");

    UUID uuid( s );

    BOOST_CHECK_EQUAL( uuid.size() , 16 );

    BOOST_CHECK( ! uuid.isNil() );

    BOOST_CHECK_EQUAL( s, uuid.asString() );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_uuid_fromstring ) {
    std::string s ("4b4053dc93e0b52a6f028cb36649d229");

    UUID uuid;

    BOOST_CHECK( uuid.isNil() );

    BOOST_CHECK_NO_THROW( uuid.fromString(s) );

    BOOST_CHECK( ! uuid.isNil() );

    BOOST_CHECK_EQUAL( s, uuid.asString() );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_types_uuid_constructor_string_trow ) {
    std::string s ("4b405");

    BOOST_CHECK_THROW( UUID uuid( s ), eckit::AssertionFailed );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
