/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iostream>

#include "eckit/utils/SHA1.h"
#include "eckit/log/Timer.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE ( "test_eckit_utils_sha1_constructor" )
{
    SHA1 hash;

    const char* msg = "The quick brown fox jumps over the lazy dog";

    hash.add(msg,strlen(msg));

    std::string res ("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");

    EXPECT( res == hash.digest() );
}

CASE ( "test_eckit_utils_sha1_constructor_string" )
{
    SHA1 hash( "Few quips galvanized the mock jury box" );

    std::string res ("4c135b5afa7f415e71a509b747c1391811fdc52c");

    EXPECT( res == hash.digest() );
}

CASE ( "test_eckit_utils_sha1_double_add" )
{
    SHA1 hash;

    const char* msg = "The quick brown fox jumps over the lazy dog";

    hash.add(msg,strlen(msg));
    hash.add(msg,strlen(msg));

    std::string res ("86c842aa0249527aad022bab2f8a9b4d77b82b12");

    EXPECT( res == hash.digest() );
}

// original test suite from RFC-3174

CASE ( "test_eckit_utils_sha1_test_suite_from_rfc3174" ) {

    /*
     *  Define patterns for testing
     */
    #define TEST1   "abc"
    #define TEST2a  "abcdbcdecdefdefgefghfghighijhi"
    #define TEST2b  "jkijkljklmklmnlmnomnopnopq"
    #define TEST2   TEST2a TEST2b
    #define TEST3   "a"
    #define TEST4a  "01234567012345670123456701234567"
    #define TEST4b  "01234567012345670123456701234567"
    /* an exact multiple of 512 bits */
    #define TEST4   TEST4a TEST4b

    const char *testarray[4] =
    {
        TEST1,
        TEST2,
        TEST3,
        TEST4
    };

    const char *resultarray[4] =
    {
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
        "34aa973cd4c4daa4f61eeb2bdbad27316534016f",
        "dea356a2cddd90c7a7ecedc5ebb563934f460452"
    };

    const size_t n[4] = { 1, 1, 1000000, 10 };

    for(int i = 0; i < 4; ++i ) {

        SHA1 hash;
        for(size_t j=0; j < n[i]; ++j) {
            hash.add(testarray[i], ::strlen(testarray[i]));
        }

        EXPECT(hash.digest() == std::string(resultarray[i]));
    }
}

CASE ( "test_eckit_utils_sha1_compute" )
{
    std::string msg ( "The quick brown fox jumps over the lazy dog" );

    std::string res ("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");

    SHA1 hash;
    EXPECT ( res == hash.compute(msg.c_str(), msg.size()));
}

CASE ( "test_eckit_utils_sha1_reset" )
{
    SHA1 hash( "FOOBAR" );

    hash.reset(); // reset initial state

    std::string msg ( "The quick brown fox jumps over the lazy dog" );

    hash.add(msg.c_str(), msg.size());

    std::string res ("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");

    EXPECT( res == hash.digest());
}

//----------------------------------------------------------------------------------------------------------------------


} // end namespace test
} // end namespace eckit

int main ( int argc, char * argv[]) {
    return run_tests( argc, argv );
}
