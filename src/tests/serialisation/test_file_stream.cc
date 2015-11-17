/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>
#include <cstdio>

#define BOOST_TEST_MODULE TestStreamable
#include "ecbuild/boost_test_framework.h"

#include "eckit/serialisation/FileStream.h"

using namespace std;
using namespace eckit;

namespace eckit {
namespace test {

struct F {
    ~F()
    {
        if (filename.exists()) filename.unlink();
    }

    static PathName filename;
};

PathName F::filename = PathName::unique( "data" );

struct TestFixture {

    TestFixture() : v_char(0),
                    v_uchar(0),
                    v_bool(0),
                    v_int(0),
                    v_uint(0),
                    v_short(0),
                    v_ushort(0),
                    v_long(0),
                    v_ulong(0),
                    v_longlong(0),
                    v_ulonglong(0),
                    v_double(0) {}

    char               v_char;
    unsigned char      v_uchar;
    bool               v_bool;
    int                v_int;
    unsigned int       v_uint;
    short              v_short;
    unsigned short     v_ushort;
    long               v_long;
    unsigned long      v_ulong;
    long long          v_longlong;
    unsigned long long v_ulonglong;
//  float              v_float;
    double             v_double;
    string             v_string;
    string             v_charp;
};

BOOST_GLOBAL_FIXTURE( F );

BOOST_FIXTURE_TEST_SUITE( TestFileStream, TestFixture )

BOOST_AUTO_TEST_CASE( write_data )
{
    BOOST_TEST_MESSAGE("Write to FileStream");
    FileStream sout( F::filename.asString().c_str(), "w" );

    sout
      << char('a')
      << (unsigned char)('b')
      << bool(true)
      << int(-20000000)
      << (unsigned int)(30000000)
      << short(-4000)
      << (unsigned short)(5000)
      << long(60000000)
      << (unsigned long)(70000000)
      << (long long)(80000000)
      << (unsigned long long)(90000000)
//    << float(3)
      << double(100000000)
      << std::string("abcdefghijklmnopqrstuvwyxz")
      << "abcdefghijklmnopqrstuvwyxz"
    ;
}

BOOST_AUTO_TEST_CASE( read_data )
{
    BOOST_TEST_MESSAGE("Read from FileStream");
    FileStream sin( F::filename.asString().c_str(), "r" );

    sin
        >> v_char
        >> v_uchar
        >> v_bool
        >> v_int
        >> v_uint
        >> v_short
        >> v_ushort
        >> v_long
        >> v_ulong
        >> v_longlong
        >> v_ulonglong
    //  >> v_float
        >> v_double
        >> v_string
        >> v_charp
    ;
}

BOOST_AUTO_TEST_CASE( check_data )
{
    BOOST_TEST_MESSAGE("Data read:");
    BOOST_TEST_MESSAGE( v_char );
    BOOST_TEST_MESSAGE( v_uchar );
    BOOST_TEST_MESSAGE( v_bool );
    BOOST_TEST_MESSAGE( v_int );
    BOOST_TEST_MESSAGE( v_uint );
    BOOST_TEST_MESSAGE( v_short );
    BOOST_TEST_MESSAGE( v_ushort );
    BOOST_TEST_MESSAGE( v_long );
    BOOST_TEST_MESSAGE( v_ulong );
    BOOST_TEST_MESSAGE( v_longlong );
    BOOST_TEST_MESSAGE( v_ulonglong );
//  BOOST_TEST_MESSAGE( v_float );
    BOOST_TEST_MESSAGE( v_double );
    BOOST_TEST_MESSAGE( v_string );
    BOOST_TEST_MESSAGE( v_charp );
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace eckit
